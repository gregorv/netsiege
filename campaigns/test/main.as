

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
    void destroy()
    {
        gameObjectRemove(m_objectId);
    }

// private:
    uint32 m_objectId;
}

class Character : Object
{
    float timeOut;
    int bla;
    string foo;
    array<string> blup;
    array<double> constants;
    Character()
    {
        debug("ctor");

    }
    ~Character()
    {
        debug("dtor");
    }

    void init()
    {
        timeOut = 10.0;
        debug("init()");
    }
    void step(float dt)
    {
        timeOut -= dt;
//         debug("step()");
        if (timeOut <= 0.0) {
            destroy();
        }
    }
    void cleanup()
    {
        debug("cleanup()");
    }

    void handleRpc()
    {
    }
}

void registerObjects()
{
    gameObjectRegister("Character", "Character", "bla, blup,foo, constants", 0);
}

void receiveChat(uint16 sender_id, string message)
{
    debug("Incoming chat Message: " + message);
}

weakref<Character> character;

void initServer()
{
    registerObjects();
    Character@ chara = cast<Character>(gameObjectCreate("Character", "geyloard"));
    character = weakref<Character>(chara);
    chara.bla = 1337;
    chara.foo = "Hallo Sync";
    chara.blup.insertLast("test");
    chara.blup.insertLast("Harry");
    chara.blup.insertLast("Wagen");
    chara.constants.insertLast(3.141);
    chara.constants.insertLast(1.6e-19);
    __rpcRegisterHandler(0, 16, "receiveChat");
}

void stepServer(float dt)
{
//     Character@ chara = cast<Character>(gameObjectFind("geyloard"));
    if (character.get() != null) {
        if (character.get().timeOut < 5.0) {
            character.get().foo = ":**:";
        }
    }
}


void initClient()
{
    registerObjects();
    __rpcRegisterHandler(0, 16, "receiveChat");
}

void stepClient(float dt)
{
    if (gameObjectExists("geyloard")) {
        Character@ chara = cast<Character>(gameObjectFind("geyloard"));
        debug(chara.foo);
    } else {
        debug("geyloard does not exist! :(");
    }
}

