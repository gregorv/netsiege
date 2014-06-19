

interface IObject2
{
    void init();
    void step(float);/*
    vec3 getPosition();
    setPosition(vec3 pos);
    void onUnload();
    void inLoad();
    void serialize();
    void deserialize();*/
}

mixin class Object : IObject {
    void register(const string &in name)
    {
        debug("register()");
        m_objectId = gameObjectCreate(this, name);
    }

    void destroy()
    {
        gameObjectRemove(m_objectId);
    }

// private:
    int m_objectId;
}

class Character : Object
{
    float timeOut;
    Character(const string &in name)
    {
        debug("ctor");
        register(name);

    }
    ~Character()
    {
        debug("dtor");
    }

    void init()
    {
        timeOut = .25;
        debug("init()");
    }
    void step(float dt)
    {
        timeOut -= dt;
        debug("step()");
        if (timeOut <= 0.0) {
            destroy();
        }
    }
    void cleanup()
    {
        debug("cleanup()");
    }
}

void initServer()
{
    Character chara("geyloard1");
    __rpcRegisterHandler(0, 16, "handleRpc", "IcL");
    __rpcRegister(16, "sendChatMessage",  "L");
}

int counter = 0;

void stepServer(float dt)
{
// 	debug("Step server simulation");
// 	float bla = dt;
// 	bla *= dt + 6;
    counter += 1;
    RPCPackage package(16);
    package.push(6);
    package.push(uint8(5));
    package.push("Hallo");
    if (counter % 10 ==  0) {
        string b("Hallo Welt!");
        rpc.sendChatMessage(0, "Hallo");
    }
}

void test(int a) {
    debug("TEST");
}

void handleRpc(int32 arg,  uint8 bla, string foo)
{
    debug("Handle RPC");
    debug(foo);
}

