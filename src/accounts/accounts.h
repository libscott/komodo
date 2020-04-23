

#include "State.h"
#include "libdevcore/OverlayDB.h"
#include "primitives/transaction.h"
#include "consensus/validation.h"


// Globally available state db
extern dev::OverlayDB pglobalaccounts;


bool ContextualCheckAccountOutputs(const CTransaction &tx, CValidationState &state);
