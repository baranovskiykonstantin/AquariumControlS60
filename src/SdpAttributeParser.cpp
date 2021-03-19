/*
 ============================================================================
 Name        : SdpAttributeParser.cpp
 Author      : Konstantin Baranovskiy
 Copyright   : GPLv3
 Description : Check an SDP attribute value and read selected parts.
 ============================================================================
 */

// INCLUDE FILES
#include <bt_sock.h>

#include "SdpAttributeParser.h"
#include "SdpAttributeParser.pan"
#include "SdpAttributeNotifier.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeParser()
// Construct a TSdpAttributeParser.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::TSdpAttributeParser(
	RArray<TSdpAttributeParser::TSdpAttributeNode>& aNodeList,
	MSdpAttributeNotifier& aObserver) :
	iObserver(aObserver),
	iNodeList(aNodeList),
	iCurrentNodeIndex(0)
	{
	iCurrentNode = iNodeList[iCurrentNodeIndex];
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::~TSdpAttributeParser()
// Destructor.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::~TSdpAttributeParser()
	{
	// No implementation needed	
	}
	
// ----------------------------------------------------------------------------
// TSdpAttributeParser::HasFinished()
// Check if parsing processed the whole list.
// ----------------------------------------------------------------------------
//
TBool TSdpAttributeParser::HasFinished() const
	{
	return (iCurrentNode.Command() == EFinished);
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::VisitAttributeValueL()
// Process a data element.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::VisitAttributeValueL(CSdpAttrValue& aValue,
	TSdpElementType aType)
	{
	switch (iCurrentNode.Command())
		{
		case ECheckType:
			CheckTypeL(aType);
			break;

		case ECheckValue:
			CheckTypeL(aType);
			CheckValueL(aValue);
			break;

		case ECheckEnd:
			User::Leave(KErrTooBig); //list element contains too many items
			break;

		case ESkip:
			break;  // no checking required

		case EReadValue:
			CheckTypeL(aType);
			ReadValueL(aValue);
			break;

		case EFinished:
			User::Leave(KErrEof); // element is after
			return; // value should have ended

		default:
			Panic(ESdpAttributeParserInvalidCommand);
		}

	AdvanceL();
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::StartListL()
// Process the start of a data element list.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::StartListL(CSdpAttrValueList& /*aList*/)
	{
	// no checks done here
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::EndListL()
// Process the end of a data element list.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::EndListL()
	{
	// check we are at the end of a list
	if (iCurrentNode.Command() != ECheckEnd)
		{
		User::Leave(KErrGeneral);
		}

	AdvanceL();
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::CheckTypeL()
// Check the type of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::CheckTypeL(TSdpElementType aElementType) const
	{
	if (iCurrentNode.Type() != aElementType)
		{
		User::Leave(KErrGeneral);
		}
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::CheckValueL()
// Check the value of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::CheckValueL(CSdpAttrValue& aValue) const
	{
	switch (aValue.Type())
		{
		case ETypeNil:
			Panic(ESdpAttributeParserNoValue);
			break;

		case ETypeUint:
			if (aValue.Uint() != (TUint) iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeInt:
			if (aValue.Int() != iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeBoolean:
			if (aValue.Bool() != iCurrentNode.Value())
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeUUID:
			if (aValue.UUID() != TUUID(iCurrentNode.Value()))
				{
				User::Leave(KErrArgument);
				}
			break;

		case ETypeDES:
		case ETypeDEA:
			Panic(ESdpAttributeParserValueIsList);
			break;

		default:
			Panic(ESdpAttributeParserValueTypeUnsupported);
			break;
		}
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::ReadValueL()
// Pass the data element value to the observer.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::ReadValueL(CSdpAttrValue& aValue) const
	{
	iObserver.FoundElementL(iCurrentNode.Value(), aValue);
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::AdvanceL()
// Advance to the next node.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::AdvanceL()
	{
	// check not at end
	if (iCurrentNode.Command() == EFinished)
		{
		User::Leave(KErrEof);
		}

	// move to the next item
	iCurrentNode = iNodeList[++iCurrentNodeIndex];
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::TSdpAttributeNode()
// constructor.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::TSdpAttributeNode::TSdpAttributeNode()
	{
	// no implementation needed
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::~TSdpAttributeNode()
// destructor.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::TSdpAttributeNode::~TSdpAttributeNode()
	{
	// no implementation needed
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::SetCommand(TNodeCommand aCommand)
// set iCommand member variable.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::TSdpAttributeNode::SetCommand(TNodeCommand aCommand)
	{
	iCommand = aCommand;
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::SetType(TSdpElementType aType)
// set iType member variable.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::TSdpAttributeNode::SetType(TSdpElementType aType)
	{
	iType = aType;
	}

// ----------------------------------------------------------------------------
// void TSdpAttributeParser::TSdpAttributeNode::SetValue(TInt aValue)
// set iValue member variable.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::TSdpAttributeNode::SetValue(TInt aValue)
	{
	iValue = aValue;
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::Command()
// get iCommand member variable value.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::TNodeCommand TSdpAttributeParser::TSdpAttributeNode::Command() const
	{
	return iCommand;
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::Type()
// get iType member variable value.
// ----------------------------------------------------------------------------
//
TSdpElementType TSdpAttributeParser::TSdpAttributeNode::Type() const
	{
	return iType;
	}

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeNode::Value()
// get iValue member variable value.
// ----------------------------------------------------------------------------
//
TInt TSdpAttributeParser::TSdpAttributeNode::Value() const
	{
	return iValue;
	}

// End of File
