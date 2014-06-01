
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

mixin class Object {
    int m_objectId;
    void register(string name)
    {
        debug("register()");
        m_objectId = createObject(this, name);
    }
}

class Character : IObject, Object
{
    Character(string name)
    {
        debug("ctor");
        register(name);
    }

    void init()
    {
        debug("init()");
    }
    void step(float dt)
    {
        debug("step()");
    }
}

void initServer()
{
    Character chara("geyloard1");
}

void stepServer(float dt)
{
// 	debug("Step server simulation");
// 	float bla = dt;
// 	bla *= dt + 6;
}

