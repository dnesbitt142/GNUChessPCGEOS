name gnuchess.app
longname "GNU Chess 6.3.0"
tokenchars "GnCh"
tokenid 0
type appl, process, single
class GNUChessProcessClass
appobj GNUChessApp
stack 49152
heapspace 16000
library geos
library ui
library ansic
resource AppResource ui-object
resource Interface ui-object
resource ChessPieces lmem read-only shared
resource AppIconMonikersResource lmem read-only shared

# Far C/C++ data references require stable segments for the entire process.
resource attack_DATA1 data fixed
resource eval_DATA1 data fixed
resource move_do_DATA1 data fixed
resource pawn_DATA1 data fixed
resource piece_DATA1 data fixed
resource pst_DATA1 data fixed
resource search_DATA1 data fixed
resource sort_DATA1 data fixed
resource square_DATA1 data fixed
resource value_DATA1 data fixed
resource vector_DATA1 data fixed
resource board_DATA1 data fixed
resource eval_DATA2 data fixed
resource fen_DATA1 data fixed
resource material_DATA1 data fixed
resource option_DATA1 data fixed
resource pawn_DATA2 data fixed
resource pst_DATA2 data fixed
resource random_DATA1 data fixed
resource random_DATA2 data fixed
resource search_full_DATA1 data fixed
resource util_DATA1 data fixed

# list_filter calls pseudo_is_legal through a raw C++ far function pointer.
resource GCMOVELEGAL code fixed read-only shared
