/*
   AngelCode Scripting Library
   Copyright (c) 2003-2006 Andreas J�nsson

   This software is provided 'as-is', without any express or implied 
   warranty. In no event will the authors be held liable for any 
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any 
   purpose, including commercial applications, and to alter it and 
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you 
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product 
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and 
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source 
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/

   Andreas J�nsson
   andreas@angelcode.com
*/


//
// as_generic.cpp
//
// This class handles the call to a function registered with asCALL_GENERIC
//

#include "as_generic.h"
#include "as_scriptfunction.h"
#include "as_objecttype.h"
#include "as_scriptengine.h"

BEGIN_AS_NAMESPACE

asCGeneric::asCGeneric(asCScriptEngine *engine, asCScriptFunction *sysFunction, void *currentObject, asDWORD *stackPointer)
{
	this->engine = engine;
	this->sysFunction = sysFunction;
	this->currentObject = currentObject;
	this->stackPointer = stackPointer;
	
	objectRegister = 0;
	returnVal = 0;
}

asCGeneric::~asCGeneric()
{
}

asIScriptEngine *asCGeneric::GetEngine()
{
	return (asIScriptEngine*)engine;
}

void *asCGeneric::GetObject()
{
	return currentObject;
}

asDWORD asCGeneric::GetArgDWord(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( dt->IsObject() )
		return 0;

	if( dt->GetSizeOnStackDWords() != 1 )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return (asDWORD)stackPointer[offset];
}

asQWORD asCGeneric::GetArgQWord(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( dt->IsObject() )
		return 0;

	if( dt->IsReference() )
		return 0;

	if( dt->GetSizeOnStackDWords() != 2 )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return *(asQWORD*)(&stackPointer[offset]);
}

float asCGeneric::GetArgFloat(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( dt->IsObject() )
		return 0;

	if( dt->GetSizeOnStackDWords() != 1 )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return *(float*)(&stackPointer[offset]);
}

double asCGeneric::GetArgDouble(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( dt->IsObject() )
		return 0;

	if( dt->GetSizeOnStackDWords() != 2 )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return *(double*)(&stackPointer[offset]);
}

void *asCGeneric::GetArgAddress(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( !dt->IsReference() && !dt->IsObjectHandle() )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return (void*)*(size_t*)(&stackPointer[offset]);
}

void *asCGeneric::GetArgObject(asUINT arg)
{
	if( arg >= (unsigned)sysFunction->parameterTypes.GetLength() )
		return 0;

	// Verify that the type is correct
	asCDataType *dt = &sysFunction->parameterTypes[arg];
	if( !dt->IsObject() )
		return 0;

	// Determine the position of the argument
	int offset = 0;
	for( asUINT n = 0; n < arg; n++ )
		offset += sysFunction->parameterTypes[n].GetSizeOnStackDWords();

	// Get the value
	return *(void**)(&stackPointer[offset]);
}

int asCGeneric::SetReturnDWord(asDWORD val)
{
	// Verify the type of the return value
	if( sysFunction->returnType.IsObject() || sysFunction->returnType.IsReference() )
		return asINVALID_TYPE;

	if( sysFunction->returnType.GetSizeOnStackDWords() != 1 )
		return asINVALID_TYPE;

// TODO: This is not the way to solve it, we need to find another way
#ifdef AS_BIG_ENDIAN
        if( sysFunction->returnType.GetSizeInMemoryBytes() == 1 )
                *(asBYTE*)&returnVal = (asBYTE)val;
        else if( sysFunction->returnType.GetSizeInMemoryBytes() == 2 )
                *(asWORD*)&returnVal = (asWORD)val;
        else
                *(asDWORD*)&returnVal = val;
#else
        // Store the value
        *(asDWORD*)&returnVal = val;
#endif

	return 0;
}

int asCGeneric::SetReturnQWord(asQWORD val)
{
	// Verify the type of the return value
	if( sysFunction->returnType.IsObject() || sysFunction->returnType.IsReference() )
		return asINVALID_TYPE;

	if( sysFunction->returnType.GetSizeOnStackDWords() != 2 )
		return asINVALID_TYPE;

	// Store the value
	returnVal = val;

	return 0;
}

int asCGeneric::SetReturnFloat(float val)
{
	// Verify the type of the return value
	if( sysFunction->returnType.IsObject() || sysFunction->returnType.IsReference() )
		return asINVALID_TYPE;

	if( sysFunction->returnType.GetSizeOnStackDWords() != 1 )
		return asINVALID_TYPE;

	// Store the value
	*(float*)&returnVal = val;

	return 0;
}

int asCGeneric::SetReturnDouble(double val)
{
	// Verify the type of the return value
	if( sysFunction->returnType.IsObject() || sysFunction->returnType.IsReference() )
		return asINVALID_TYPE;

	if( sysFunction->returnType.GetSizeOnStackDWords() != 2 )
		return asINVALID_TYPE;

	// Store the value
	*(double*)&returnVal = val;

	return 0;
}

int asCGeneric::SetReturnAddress(void *val)
{
	// Verify the type of the return value
	if( sysFunction->returnType.IsReference() )
	{
		// Store the value
		*(void**)&returnVal = val;
		return 0;
	}
	else if( sysFunction->returnType.IsObjectHandle() )
	{
		// Store the handle without increasing reference
		objectRegister = val;
		return 0;
	}

	return asINVALID_TYPE;
}

int asCGeneric::SetReturnObject(void *obj)
{
	asCDataType *dt = &sysFunction->returnType;
	if( !dt->IsObject() )
		return asINVALID_TYPE;

	if( dt->IsReference() )
	{
		*(void**)&returnVal = obj;
		return 0;
	}

	if( dt->IsObjectHandle() )
	{
		// Increase the reference counter
		asSTypeBehaviour *beh = &dt->GetObjectType()->beh;
		if( beh->addref )
			engine->CallObjectMethod(obj, beh->addref);
	}
	else
	{
		// Make a copy of the object
		char *mem = (char*)engine->CallAlloc(dt->GetObjectType());

		// Call the object's default constructor
		asSTypeBehaviour *beh = &dt->GetObjectType()->beh;
		if( beh->construct )
			engine->CallObjectMethod(mem, beh->construct);

		// Call the object's assignment operator
		if( beh->copy )
			engine->CallObjectMethod(mem, obj, beh->copy);
		else
		{
			// Default operator is a simple copy
			memcpy(mem, obj, dt->GetSizeInMemoryBytes());
		}

		obj = mem;
	}

	objectRegister = obj;

	return 0;
}

END_AS_NAMESPACE
