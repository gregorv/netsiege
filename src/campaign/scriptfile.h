/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CAMPAIGN_SCRIPTFILE_H
#define CAMPAIGN_SCRIPTFILE_H

#include <OGRE/OgreResource.h>
#include <string.h>

namespace campaign {

class ScriptFile : public Ogre::Resource
{
public:
    ScriptFile(Ogre::ResourceManager* creator, const Ogre::String& name,
               Ogre::ResourceHandle handle, const Ogre::String& group,
               bool isManual = false, Ogre::ManualResourceLoader* loader = 0);
    ~ScriptFile();

    std::string code() const { return m_code; }

protected:
    virtual void unloadImpl(void );
    virtual void loadImpl(void );
    virtual size_t calculateSize(void ) const;

private:
    std::string m_code;
};

typedef Ogre::SharedPtr<ScriptFile> ScriptFilePtr;
/*
class ScriptFilePtr : public Ogre::SharedPtr<ScriptFile>
 {
 public:
     ScriptFilePtr() : Ogre::SharedPtr<ScriptFile>() {}
     explicit ScriptFilePtr(ScriptFilePtr *rep) : Ogre::SharedPtr<ScriptFile>(rep) {}
     ScriptFilePtr(const ScriptFilePtr &r) : Ogre::SharedPtr<ScriptFile>(r) {}
     ScriptFilePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<ScriptFile>()
     {
         if( r.isNull() )
             return;
         // lock & copy other mutex pointer
         OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
         OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
         pRep = static_cast<ScriptFile*>(r.getPointer());
         pUseCount = r.useCountPointer();
         useFreeMethod = r.freeMethod();
         if (pUseCount)
         {
             ++(*pUseCount);
         }
     }

     /// Operator used to convert a ResourcePtr to a ScriptFilePtr
     ScriptFilePtr& operator=(const Ogre::ResourcePtr& r)
     {
         if(pRep == static_cast<ScriptFile*>(r.getPointer()))
             return *this;
         release();
         if( r.isNull() )
             return *this; // resource ptr is null, so the call to release above has done all we need to do.
         // lock & copy other mutex pointer
         OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
         OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
         pRep = static_cast<ScriptFile*>(r.getPointer());
         pUseCount = r.useCountPointer();
         useFreeMethod = r.freeMethod();
         if (pUseCount)
         {
             ++(*pUseCount);
         }
         return *this;
     }
 };*/

}

#endif // CAMPAIGN_SCRIPTFILE_H
