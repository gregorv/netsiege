#ifndef SCRIPTDICTIONARY_H
#define SCRIPTDICTIONARY_H

// The dictionary class relies on the script string object, thus the script
// string type must be registered with the engine before registering the
// dictionary type

#ifndef ANGELSCRIPT_H 
// Avoid having to inform include path if header is already include before
#include <angelscript.h>
#endif

#include <string>

#ifdef _MSC_VER
// Turn off annoying warnings about truncated symbol names
#pragma warning (disable:4786)
#endif

#include <map>

// Sometimes it may be desired to use the same method names as used by C++ STL.
// This may for example reduce time when converting code from script to C++ or
// back.
//
//  0 = off
//  1 = on

#ifndef AS_USE_STLNAMES
#define AS_USE_STLNAMES 0
#endif


BEGIN_AS_NAMESPACE

class CScriptArray;

class CScriptDictionary
{
public:
	// Constructor
	CScriptDictionary(asIScriptEngine *engine);

	// Constructor. Called from the script to instantiate a dictionary from an initialization list
	CScriptDictionary(asBYTE *buffer);

	// Reference counting
	void AddRef() const;
	void Release() const;

	// Reassign the dictionary
	CScriptDictionary &operator =(const CScriptDictionary &other);

	// Sets a key/value pair
	void Set(const std::string &key, void *value, int typeId);
	void Set(const std::string &key, const asINT64 &value);
	void Set(const std::string &key, const double &value);

	// Gets the stored value. Returns false if the value isn't compatible with the informed typeId
	bool Get(const std::string &key, void *value, int typeId) const;
	bool Get(const std::string &key, asINT64 &value) const;
	bool Get(const std::string &key, double &value) const;

	// Returns the type id of the stored value, or negative if it doesn't exist
	int  GetTypeId(const std::string &key) const;

	// Returns true if the key is set
	bool Exists(const std::string &key) const;

	// Returns true if there are no key/value pairs in the dictionary
	bool IsEmpty() const;

	// Returns the number of key/value pairs in the dictionary
	asUINT GetSize() const;

	// Deletes the key
	void Delete(const std::string &key);

	// Deletes all keys
	void DeleteAll();

	// Get an array of all keys
	CScriptArray *GetKeys() const;

	// STL style iterator
protected:
	struct valueStruct;
public:
	class CIterator
	{
	public:
		void operator++();    // Pre-increment
		void operator++(int); // Post-increment

		// This is needed to support C++11 range-for
		CIterator &operator*();

		bool operator==(const CIterator &other) const;
		bool operator!=(const CIterator &other) const;

		// Accessors
		const std::string &GetKey() const;
		int                GetTypeId() const;
		bool               GetValue(asINT64 &value) const;
		bool               GetValue(double &value) const;
		bool               GetValue(void *value, int typeId) const;

	protected:
		friend class CScriptDictionary;

		CIterator();
		CIterator(const CScriptDictionary &dict,
		          std::map<std::string, CScriptDictionary::valueStruct>::const_iterator it);

		std::map<std::string, CScriptDictionary::valueStruct>::const_iterator m_it;
		const CScriptDictionary &m_dict;
	};

	CIterator begin() const;
	CIterator end() const;

	// Garbage collections behaviours
	int GetRefCount();
	void SetGCFlag();
	bool GetGCFlag();
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseAllReferences(asIScriptEngine *engine);

protected:
	// We don't want anyone to call the destructor directly, it should be called through the Release method
	virtual ~CScriptDictionary();

	// Helper methods
	void FreeValue(valueStruct &value);
	bool GetValue(const valueStruct &vs, void *value, int typeId) const;
	
	// Our properties
	asIScriptEngine *engine;
	mutable int refCount;
	mutable bool gcFlag;

	// The structure for holding the values
	struct valueStruct
	{
		union
		{
			asINT64 valueInt;
			double  valueFlt;
			void   *valueObj;
		};
		int   typeId;
	};

	// TODO: optimize: Use C++11 std::unordered_map instead
	std::map<std::string, valueStruct> dict;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the dictionary object
void RegisterScriptDictionary(asIScriptEngine *engine);

// Call this function to register the math functions
// using native calling conventions
void RegisterScriptDictionary_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptDictionary_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
