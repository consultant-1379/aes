/*******************************************************************************/
/**
@file transactionrules.cpp

Class method implementation for transactionrules.h

DESCRIPTION
The services provides transaction rules.

ERROR HANDLING

General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <transactionrules.h>


/*===================================================================
   ROUTINE:TransactionRules
=================================================================== */
TransactionRules::TransactionRules() : next_(TR_GETLASTCOMMITTEDBLOCKNO),
    prev_(TR_TRANSACTIONTERMINATE)
{}


/*===================================================================
   ROUTINE:~TransactionRules
=================================================================== */
TransactionRules::~TransactionRules()
{}


/*===================================================================
   ROUTINE:setNext
=================================================================== */
void TransactionRules::setNext()
{
    switch (next_)
    {
/*        case TR_TRANSACTIONBEGIN:
        {
            next_ = (Rules)(TR_TRANSACTIONEND | TR_TRANSACTIONTERMINATE);
            prev_ = TR_TRANSACTIONBEGIN;
            break;
        } */

        case TR_TRANSACTIONBEGIN:
        {
            // At least one block must have been sent in order
            // for TR_TRANSACTIONEND to be valid
            next_ = TR_TRANSACTIONTERMINATE;
            prev_ = TR_TRANSACTIONBEGIN;
            break;
        }

        case TR_TRANSACTIONEND:
        {
            next_ = (Rules)(TR_TRANSACTIONCOMMIT | TR_TRANSACTIONTERMINATE);
            prev_ = TR_TRANSACTIONEND;
            break;
        }

        case TR_TRANSACTIONCOMMIT:
        {
            next_ = TR_TRANSACTIONBEGIN;
            prev_ = TR_TRANSACTIONCOMMIT;
            break;
        }

        case TR_TRANSACTIONTERMINATE:
        {
            next_ = TR_GETLASTCOMMITTEDBLOCKNO;            // What should happen here??
            prev_ = TR_TRANSACTIONTERMINATE;
            break;
        }

        case TR_GETLASTCOMMITTEDBLOCKNO:
        {
            next_ = TR_TRANSACTIONBEGIN;
            prev_ = TR_GETLASTCOMMITTEDBLOCKNO;
            break;
        }

        default:
            break;
    }
}


/*===================================================================
   ROUTINE:checkNext
=================================================================== */
bool TransactionRules::checkNext(Rules next)
{
    if ( next_ == next )
        return true;

    // TR_TRANSACTIONEND and TR_TRANSACTIONTERMINATE are ONLY valid if
    // at least one block has been transferred!
    if ( prev_ == TR_TRANSACTIONBEGIN )
    {
        bool b1 = ((next_ & TR_TRANSACTIONEND) == next );
        bool b2 = ((next_ & TR_TRANSACTIONTERMINATE) == next );

        if (b1)
            next_ = TR_TRANSACTIONEND;
        else if (b2)
            next_ = TR_TRANSACTIONTERMINATE;

        if ( (b1 == true) || (b2 == true) )
            return true;

        return false;
    }
  
    
    // Both TR_TRANSACTIONCOMMIT and TR_TRANSACTIONTERMINATE are
    // valid after TR_TRANSACTIONEND
    if ( prev_ == TR_TRANSACTIONEND )
    {
        bool b1 = ((next_ & TR_TRANSACTIONCOMMIT) == next );
        bool b2 = ((next_ & TR_TRANSACTIONTERMINATE) == next );
        
        if (b1)
            next_ = TR_TRANSACTIONCOMMIT;
        else if (b2)
            next_ = TR_TRANSACTIONTERMINATE;

        if ( (b1 == true) || (b2 == true ))
            return true;
        
        return false;
    }

    return false;
}


/*===================================================================
   ROUTINE:blockSent
=================================================================== */
bool TransactionRules::blockSent()
{
    if ( prev_ != TR_TRANSACTIONBEGIN )
        return false;

    next_ = (Rules)(TR_TRANSACTIONEND | TR_TRANSACTIONTERMINATE);
    return true;
}
