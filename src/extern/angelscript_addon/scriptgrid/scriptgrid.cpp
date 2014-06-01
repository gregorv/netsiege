#include <new>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h> // sprintf

#include "scriptgrid.h"

using namespace std;

BEGIN_AS_NAMESPACE

// Set the default memory routines
static asALLOCFUNC_t userAlloc = ::malloc;
static asFREEFUNC_t  userFree  = ::free;

// Allows the application to set which memory routines should be used by the array object
void CScriptGrid::SetMemoryFunctions(asALLOCFUNC_t allocFunc, asFREEFUNC_t freeFunc)
{
	userAlloc = allocFunc;
	userFree = freeFunc;
}

static void RegisterScriptGrid_Native(asIScriptEngine *engine);

struct SGridBuffer
{
	asDWORD width;
	asDWORD height;
	asBYTE  data[1];
};

CScriptGrid* CScriptGrid::Create(asIObjectType *ot, asUINT w, asUINT h)
{
	asIScriptContext *ctx = asGetActiveContext();

	// Allocate the memory
	void *mem = userAlloc(sizeof(CScriptGrid));
	if( mem == 0 )
	{
		if( ctx )
			ctx->SetException("Out of memory");

		return 0;
	}

	// Initialize the object
	CScriptGrid *a = new(mem) CScriptGrid(w, h, ot);

	// It's possible the constructor raised a script exception, in which case we
	// need to free the memory and return null instead, else we get a memory leak.
	if( ctx && ctx->GetState() == asEXECUTION_EXCEPTION )
	{
		a->Release();
		return 0;
	}

	return a;
}

CScriptGrid* CScriptGrid::Create(asIObjectType *ot, void *initList)
{
	asIScriptContext *ctx = asGetActiveContext();

	// Allocate the memory
	void *mem = userAlloc(sizeof(CScriptGrid));
	if( mem == 0 )
	{
		if( ctx )
			ctx->SetException("Out of memory");

		return 0;
	}

	// Initialize the object
	CScriptGrid *a = new(mem) CScriptGrid(ot, initList);

	// It's possible the constructor raised a script exception, in which case we
	// need to free the memory and return null instead, else we get a memory leak.
	if( ctx && ctx->GetState() == asEXECUTION_EXCEPTION )
	{
		a->Release();
		return 0;
	}

	return a;
}

CScriptGrid* CScriptGrid::Create(asIObjectType *ot, asUINT w, asUINT h, void *defVal)
{
	asIScriptContext *ctx = asGetActiveContext();

	// Allocate the memory
	void *mem = userAlloc(sizeof(CScriptGrid));
	if( mem == 0 )
	{
		if( ctx )
			ctx->SetException("Out of memory");

		return 0;
	}

	// Initialize the object
	CScriptGrid *a = new(mem) CScriptGrid(w, h, defVal, ot);

	// It's possible the constructor raised a script exception, in which case we
	// need to free the memory and return null instead, else we get a memory leak.
	if( ctx && ctx->GetState() == asEXECUTION_EXCEPTION )
	{
		a->Release();
		return 0;
	}

	return a;
}

// This optional callback is called when the template type is first used by the compiler.
// It allows the application to validate if the template can be instanciated for the requested
// subtype at compile time, instead of at runtime. The output argument dontGarbageCollect
// allow the callback to tell the engine if the template instance type shouldn't be garbage collected,
// i.e. no asOBJ_GC flag.
static bool ScriptGridTemplateCallback(asIObjectType *ot, bool &dontGarbageCollect)
{
	// Make sure the subtype can be instanciated with a default factory/constructor,
	// otherwise we won't be able to instanciate the elements.
	int typeId = ot->GetSubTypeId();
	if( typeId == asTYPEID_VOID )
		return false;
	if( (typeId & asTYPEID_MASK_OBJECT) && !(typeId & asTYPEID_OBJHANDLE) )
	{
		asIObjectType *subtype = ot->GetEngine()->GetObjectTypeById(typeId);
		asDWORD flags = subtype->GetFlags();
		if( (flags & asOBJ_VALUE) && !(flags & asOBJ_POD) )
		{
			// Verify that there is a default constructor
			bool found = false;
			for( asUINT n = 0; n < subtype->GetBehaviourCount(); n++ )
			{
				asEBehaviours beh;
				asIScriptFunction *func = subtype->GetBehaviourByIndex(n, &beh);
				if( beh != asBEHAVE_CONSTRUCT ) continue;

				if( func->GetParamCount() == 0 )
				{
					// Found the default constructor
					found = true;
					break;
				}
			}

			if( !found )
			{
				// There is no default constructor
				return false;
			}
		}
		else if( (flags & asOBJ_REF) )
		{
			bool found = false;

			// If value assignment for ref type has been disabled then the array
			// can be created if the type has a default factory function
			if( !ot->GetEngine()->GetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE) )
			{
				// Verify that there is a default factory
				for( asUINT n = 0; n < subtype->GetFactoryCount(); n++ )
				{
					asIScriptFunction *func = subtype->GetFactoryByIndex(n);
					if( func->GetParamCount() == 0 )
					{
						// Found the default factory
						found = true;
						break;
					}
				}
			}

			if( !found )
			{
				// No default factory
				return false;
			}
		}

		// If the object type is not garbage collected then the array also doesn't need to be
		if( !(flags & asOBJ_GC) )
			dontGarbageCollect = true;
	}
	else if( !(typeId & asTYPEID_OBJHANDLE) )
	{
		// Arrays with primitives cannot form circular references,
		// thus there is no need to garbage collect them
		dontGarbageCollect = true;
	}

	// The type is ok
	return true;
}

// Registers the template array type
void RegisterScriptGrid(asIScriptEngine *engine)
{
	// TODO: Implement the generic calling convention
	RegisterScriptGrid_Native(engine);
}

static void RegisterScriptGrid_Native(asIScriptEngine *engine)
{
	int r;

	// Register the grid type as a template
	r = engine->RegisterObjectType("grid<class T>", 0, asOBJ_REF | asOBJ_GC | asOBJ_TEMPLATE); assert( r >= 0 );

	// Register a callback for validating the subtype before it is used
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_TEMPLATE_CALLBACK, "bool f(int&in, bool&out)", asFUNCTION(ScriptGridTemplateCallback), asCALL_CDECL); assert( r >= 0 );

	// Templates receive the object type as the first parameter. To the script writer this is hidden
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_FACTORY, "grid<T>@ f(int&in, uint, uint)", asFUNCTIONPR(CScriptGrid::Create, (asIObjectType*, asUINT, asUINT), CScriptGrid*), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_FACTORY, "grid<T>@ f(int&in, uint, uint, const T &in)", asFUNCTIONPR(CScriptGrid::Create, (asIObjectType*, asUINT, asUINT, void *), CScriptGrid*), asCALL_CDECL); assert( r >= 0 );

	// Register the factory that will be used for initialization lists
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_LIST_FACTORY, "grid<T>@ f(int&in type, int&in list) {repeat {repeat_same T}}", asFUNCTIONPR(CScriptGrid::Create, (asIObjectType*, void*), CScriptGrid*), asCALL_CDECL); assert( r >= 0 );

	// The memory management methods
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptGrid,AddRef), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptGrid,Release), asCALL_THISCALL); assert( r >= 0 );

	// The index operator returns the template subtype
	r = engine->RegisterObjectMethod("grid<T>", "T &opIndex(uint, uint)", asMETHODPR(CScriptGrid, At, (asUINT, asUINT), void*), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("grid<T>", "const T &opIndex(uint, uint) const", asMETHODPR(CScriptGrid, At, (asUINT, asUINT) const, const void*), asCALL_THISCALL); assert( r >= 0 );

	// Other methods
	r = engine->RegisterObjectMethod("grid<T>", "uint width() const", asMETHOD(CScriptGrid, GetWidth), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("grid<T>", "uint height() const", asMETHOD(CScriptGrid, GetHeight), asCALL_THISCALL); assert( r >= 0 );

	// Register GC behaviours in case the array needs to be garbage collected
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(CScriptGrid, GetRefCount), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_SETGCFLAG, "void f()", asMETHOD(CScriptGrid, SetFlag), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(CScriptGrid, GetFlag), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(CScriptGrid, EnumReferences), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("grid<T>", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(CScriptGrid, ReleaseAllHandles), asCALL_THISCALL); assert( r >= 0 );
}

CScriptGrid::CScriptGrid(asIObjectType *ot, void *buf)
{
	refCount = 1;
	gcFlag = false;
	objType = ot;
	objType->AddRef();
	buffer = 0;
	subTypeId = objType->GetSubTypeId();

	asIScriptEngine *engine = ot->GetEngine();

	// Determine element size
	if( subTypeId & asTYPEID_MASK_OBJECT )
		elementSize = sizeof(asPWORD);
	else
		elementSize = engine->GetSizeOfPrimitiveType(subTypeId);

	// Determine the initial size from the buffer
	asUINT height = *(asUINT*)buf;
	asUINT width = *(asUINT*)((char*)(buf)+4);

	// Make sure the grid size isn't too large for us to handle
	if( !CheckMaxSize(width, height) )
	{
		// Don't continue with the initialization
		return;
	}

	// Skip the height value at the start of the buffer
	buf = (asUINT*)(buf)+1;

	// Copy the values of the grid elements from the buffer
	if( (ot->GetSubTypeId() & asTYPEID_MASK_OBJECT) == 0 )
	{
		CreateBuffer(&buffer, width, height);

		// Copy the values of the primitive type into the internal buffer
		for( asUINT y = 0; y < height; y++ )
		{
			// Skip the length value at the start of each row
			buf = (asUINT*)(buf)+1;

			// Copy the line
			memcpy(At(0,y), buf, width*elementSize);

			// Move to next line
			buf = (char*)(buf) + width*elementSize;

			// Align to 4 byte boundary
			if( asPWORD(buf) & 0x3 )
				buf = (char*)(buf) + 4 - (asPWORD(buf) & 0x3);
		}
	}
	else if( ot->GetSubTypeId() & asTYPEID_OBJHANDLE )
	{
		CreateBuffer(&buffer, width, height);

		// Copy the handles into the internal buffer
		for( asUINT y = 0; y < height; y++ )
		{
			// Skip the length value at the start of each row
			buf = (asUINT*)(buf)+1;

			// Copy the line
			memcpy(At(0,y), buf, width*elementSize);

			// With object handles it is safe to clear the memory in the received buffer
			// instead of increasing the ref count. It will save time both by avoiding the
			// call the increase ref, and also relieve the engine from having to release
			// its references too
			memset(buf, 0, width*elementSize);

			// Move to next line
			buf = (char*)(buf) + width*elementSize;

			// Align to 4 byte boundary
			if( asPWORD(buf) & 0x3 )
				buf = (char*)(buf) + 4 - (asPWORD(buf) & 0x3);
		}
	}
	else if( ot->GetSubType()->GetFlags() & asOBJ_REF )
	{
		// Only allocate the buffer, but not the objects
		subTypeId |= asTYPEID_OBJHANDLE;
		CreateBuffer(&buffer, width, height);
		subTypeId &= ~asTYPEID_OBJHANDLE;

		// Copy the handles into the internal buffer
		for( asUINT y = 0; y < height; y++ )
		{
			// Skip the length value at the start of each row
			buf = (asUINT*)(buf)+1;

			// Copy the line
			memcpy(At(0,y), buf, width*elementSize);

			// With object handles it is safe to clear the memory in the received buffer
			// instead of increasing the ref count. It will save time both by avoiding the
			// call the increase ref, and also relieve the engine from having to release
			// its references too
			memset(buf, 0, width*elementSize);

			// Move to next line
			buf = (char*)(buf) + width*elementSize;

			// Align to 4 byte boundary
			if( asPWORD(buf) & 0x3 )
				buf = (char*)(buf) + 4 - (asPWORD(buf) & 0x3);
		}
	}
	else
	{
		// TODO: Optimize by calling the copy constructor of the object instead of
		//       constructing with the default constructor and then assigning the value
		// TODO: With C++11 ideally we should be calling the move constructor, instead
		//       of the copy constructor as the engine will just discard the objects in the
		//       buffer afterwards.
		CreateBuffer(&buffer, width, height);

		// For value types we need to call the opAssign for each individual object
		asIObjectType *subType = ot->GetSubType();
		asUINT subTypeSize = subType->GetSize();
		for( asUINT y = 0;y < height; y++ )
		{
			// Skip the length value at the start of each row
			buf = (asUINT*)(buf)+1;

			// Call opAssign for each of the objects on the row
			for( asUINT x = 0; x < width; x++ )
			{
				void *obj = At(x,y);
				asBYTE *srcObj = (asBYTE*)(buf) + x*subTypeSize;
				engine->AssignScriptObject(obj, srcObj, subType);
			}

			// Move to next line
			buf = (char*)(buf) + width*subTypeSize;

			// Align to 4 byte boundary
			if( asPWORD(buf) & 0x3 )
				buf = (char*)(buf) + 4 - (asPWORD(buf) & 0x3);
		}
	}

	// Notify the GC of the successful creation
	if( objType->GetFlags() & asOBJ_GC )
		objType->GetEngine()->NotifyGarbageCollectorOfNewObject(this, objType);
}

CScriptGrid::CScriptGrid(asUINT width, asUINT height, asIObjectType *ot)
{
	refCount = 1;
	gcFlag = false;
	objType = ot;
	objType->AddRef();
	buffer = 0;
	subTypeId = objType->GetSubTypeId();

	// Determine element size
	if( subTypeId & asTYPEID_MASK_OBJECT )
		elementSize = sizeof(asPWORD);
	else
		elementSize = objType->GetEngine()->GetSizeOfPrimitiveType(subTypeId);

	// Make sure the array size isn't too large for us to handle
	if( !CheckMaxSize(width, height) )
	{
		// Don't continue with the initialization
		return;
	}

	CreateBuffer(&buffer, width, height);

	// Notify the GC of the successful creation
	if( objType->GetFlags() & asOBJ_GC )
		objType->GetEngine()->NotifyGarbageCollectorOfNewObject(this, objType);
}

CScriptGrid::CScriptGrid(asUINT width, asUINT height, void *defVal, asIObjectType *ot)
{
	refCount = 1;
	gcFlag = false;
	objType = ot;
	objType->AddRef();
	buffer = 0;
	subTypeId = objType->GetSubTypeId();

	// Determine element size
	if( subTypeId & asTYPEID_MASK_OBJECT )
		elementSize = sizeof(asPWORD);
	else
		elementSize = objType->GetEngine()->GetSizeOfPrimitiveType(subTypeId);

	// Make sure the array size isn't too large for us to handle
	if( !CheckMaxSize(width, height) )
	{
		// Don't continue with the initialization
		return;
	}

	CreateBuffer(&buffer, width, height);

	// Notify the GC of the successful creation
	if( objType->GetFlags() & asOBJ_GC )
		objType->GetEngine()->NotifyGarbageCollectorOfNewObject(this, objType);

	// Initialize the elements with the default value
	for( asUINT y = 0; y < GetHeight(); y++ )
		for( asUINT x = 0; x < GetWidth(); x++ )
			SetValue(x, y, defVal);
}

void CScriptGrid::SetValue(asUINT x, asUINT y, void *value)
{
	// At() will take care of the out-of-bounds checking, though
	// if called from the application then nothing will be done
	void *ptr = At(x, y);
	if( ptr == 0 ) return;

	if( (subTypeId & ~asTYPEID_MASK_SEQNBR) && !(subTypeId & asTYPEID_OBJHANDLE) )
		objType->GetEngine()->AssignScriptObject(ptr, value, objType->GetSubType());
	else if( subTypeId & asTYPEID_OBJHANDLE )
	{
		void *tmp = *(void**)ptr;
		*(void**)ptr = *(void**)value;
		objType->GetEngine()->AddRefScriptObject(*(void**)value, objType->GetSubType());
		if( tmp )
			objType->GetEngine()->ReleaseScriptObject(tmp, objType->GetSubType());
	}
	else if( subTypeId == asTYPEID_BOOL ||
			 subTypeId == asTYPEID_INT8 ||
			 subTypeId == asTYPEID_UINT8 )
		*(char*)ptr = *(char*)value;
	else if( subTypeId == asTYPEID_INT16 ||
			 subTypeId == asTYPEID_UINT16 )
		*(short*)ptr = *(short*)value;
	else if( subTypeId == asTYPEID_INT32 ||
			 subTypeId == asTYPEID_UINT32 ||
			 subTypeId == asTYPEID_FLOAT ||
			 subTypeId > asTYPEID_DOUBLE ) // enums have a type id larger than doubles
		*(int*)ptr = *(int*)value;
	else if( subTypeId == asTYPEID_INT64 ||
			 subTypeId == asTYPEID_UINT64 ||
			 subTypeId == asTYPEID_DOUBLE )
		*(double*)ptr = *(double*)value;
}

CScriptGrid::~CScriptGrid()
{
	if( buffer )
	{
		DeleteBuffer(buffer);
		buffer = 0;
	}
	if( objType ) objType->Release();
}

asUINT CScriptGrid::GetWidth() const
{
	return buffer->width;
}

asUINT CScriptGrid::GetHeight() const
{
	return buffer->height;
}

// internal
bool CScriptGrid::CheckMaxSize(asUINT width, asUINT height)
{
	// This code makes sure the size of the buffer that is allocated
	// for the array doesn't overflow and becomes smaller than requested

	asUINT maxSize = 0xFFFFFFFFul - sizeof(SGridBuffer) + 1;
	if( elementSize > 0 )
		maxSize /= elementSize;

	asINT64 numElements  = width * height;

	if( (numElements >> 32) || numElements > maxSize )
	{
		asIScriptContext *ctx = asGetActiveContext();
		if( ctx )
			ctx->SetException("Too large grid size");

		return false;
	}

	// OK
	return true;
}

asIObjectType *CScriptGrid::GetGridObjectType() const
{
	return objType;
}

int CScriptGrid::GetGridTypeId() const
{
	return objType->GetTypeId();
}

int CScriptGrid::GetElementTypeId() const
{
	return subTypeId;
}

// Return a pointer to the array element. Returns 0 if the index is out of bounds
const void *CScriptGrid::At(asUINT x, asUINT y) const
{
	if( buffer == 0 || x >= buffer->width || y >= buffer->height )
	{
		// If this is called from a script we raise a script exception
		asIScriptContext *ctx = asGetActiveContext();
		if( ctx )
			ctx->SetException("Index out of bounds");
		return 0;
	}

	asUINT index = x+y*buffer->width;
	if( (subTypeId & asTYPEID_MASK_OBJECT) && !(subTypeId & asTYPEID_OBJHANDLE) )
		return *(void**)(buffer->data + elementSize*index);
	else
		return buffer->data + elementSize*index;
}
void *CScriptGrid::At(asUINT x, asUINT y)
{
	return const_cast<void*>(const_cast<const CScriptGrid *>(this)->At(x, y));
}


// internal
void CScriptGrid::CreateBuffer(SGridBuffer **buf, asUINT w, asUINT h)
{
	asUINT numElements = w * h;

	*buf = reinterpret_cast<SGridBuffer*>(userAlloc(sizeof(SGridBuffer)-1+elementSize*numElements));

	if( *buf )
	{
		(*buf)->width  = w;
		(*buf)->height = h;
		Construct(*buf);
	}
	else
	{
		// Oops, out of memory
		asIScriptContext *ctx = asGetActiveContext();
		if( ctx )
			ctx->SetException("Out of memory");
	}
}

// internal
void CScriptGrid::DeleteBuffer(SGridBuffer *buf)
{
	Destruct(buf);

	// Free the buffer
	userFree(buf);
}

// internal
void CScriptGrid::Construct(SGridBuffer *buf)
{
	if( subTypeId & asTYPEID_OBJHANDLE )
	{
		// Set all object handles to null
		void *d = (void*)(buf->data);
		memset(d, 0, (buf->width*buf->height)*sizeof(void*));
	}
	else if( subTypeId & asTYPEID_MASK_OBJECT )
	{
		void **max = (void**)(buf->data + (buf->width*buf->height) * sizeof(void*));
		void **d = (void**)(buf->data);

		asIScriptEngine *engine = objType->GetEngine();
		asIObjectType *subType = objType->GetSubType();

		for( ; d < max; d++ )
			*d = (void*)engine->CreateScriptObject(subType);
	}
}

// internal
void CScriptGrid::Destruct(SGridBuffer *buf)
{
	if( subTypeId & asTYPEID_MASK_OBJECT )
	{
		asIScriptEngine *engine = objType->GetEngine();

		void **max = (void**)(buf->data + (buf->width*buf->height) * sizeof(void*));
		void **d   = (void**)(buf->data);

		for( ; d < max; d++ )
		{
			if( *d )
				engine->ReleaseScriptObject(*d, objType->GetSubType());
		}
	}
}

// GC behaviour
void CScriptGrid::EnumReferences(asIScriptEngine *engine)
{
	// If the array is holding handles, then we need to notify the GC of them
	if( subTypeId & asTYPEID_MASK_OBJECT )
	{
		asUINT numElements = buffer->width * buffer->height;
		void **d = (void**)buffer->data;
		for( asUINT n = 0; n < numElements; n++ )
		{
			if( d[n] )
				engine->GCEnumCallback(d[n]);
		}
	}
}

// GC behaviour
void CScriptGrid::ReleaseAllHandles(asIScriptEngine *engine)
{
	DeleteBuffer(buffer);
	buffer = 0;
}

void CScriptGrid::AddRef() const
{
	// Clear the GC flag then increase the counter
	gcFlag = false;
	asAtomicInc(refCount);
}

void CScriptGrid::Release() const
{
	// Clearing the GC flag then descrease the counter
	gcFlag = false;
	if( asAtomicDec(refCount) == 0 )
	{
		// When reaching 0 no more references to this instance
		// exists and the object should be destroyed
		this->~CScriptGrid();
		userFree(const_cast<CScriptGrid*>(this));
	}
}

// GC behaviour
int CScriptGrid::GetRefCount()
{
	return refCount;
}

// GC behaviour
void CScriptGrid::SetFlag()
{
	gcFlag = true;
}

// GC behaviour
bool CScriptGrid::GetFlag()
{
	return gcFlag;
}

END_AS_NAMESPACE
