// TODO: we want some 'server system' that supports a number of operations:
// - Adding a scene,
// - Updating images / slices.
// - Benchmark data?
//
// I guess we want to use OpenMQ for this. Do we want two-way communication or
// just 'fire'? The nice thing about 'two-way' is that this 'slice
// reconstruction' thing can also be done. At first simply only 'sending' the
// appropriate slice, but later restricting construction only to the request.

#include <cstring>
#include <memory>
#include <queue>
#include <thread>

#include "ticker.hpp"
#include "serialize.hpp"

namespace tomovis {

enum class packet_desc : int {
    // making a scene is done by supplying a name and some
    // information on the scene, e.g.
    // 2d or 3d, or dimensions
    // the scene_id will be returned
    make_scene,
    update_image
};

class Packet {
   public:
    Packet(packet_desc desc_) : desc(desc_) {}
    packet_desc desc;

    virtual memory_buffer serialize() = 0;
    virtual void deserialize(memory_buffer buffer) = 0;
};

class MakeScenePacket : public Packet {
   public:
    MakeScenePacket() : Packet(packet_desc::make_scene) {}
    MakeScenePacket(std::string name_) : Packet(packet_desc::make_scene), name(name_) {}

    std::size_t size() {
        scale total;
        fill(total);
        return total.size;
    }

    memory_buffer serialize() override {
        auto result = memory_buffer(size());
        fill(result);
        return result;
    }

    // merge this and deserialize?
    template <typename Buffer>
    void fill(Buffer& buffer) {
        buffer << this->desc;
        buffer << name;
    }

    void deserialize(memory_buffer buffer) override {
        buffer >> this->desc;
        buffer >> name;
    }

    std::string name;
    int scene_id;
};

class SceneList;

class Server : public Ticker {
   public:
    Server(SceneList& scenes) : scenes_(scenes) {}

    void start();
    void tick(float) override;

   private:
    /** The server should have access to the list of scenes, to obtain the
     * correct one, or add a new one. */ SceneList& scenes_;
    std::thread server_thread;

    /** Filled by server thread, performed by 'OpenGL' tread */
    std::queue<std::unique_ptr<Packet>> packets_;
};

} // namespace tomovis
