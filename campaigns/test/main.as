
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
}

void stepServer(float dt)
{
// 	debug("Step server simulation");
// 	float bla = dt;
// 	bla *= dt + 6;
}

