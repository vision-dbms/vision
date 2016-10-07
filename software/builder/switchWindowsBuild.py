import os, sys, argparse, stat, traceback, shutil, subprocess, logging, json
import glob


logging.basicConfig(level=logging.WARNING)
log = logging.getLogger(os.path.basename(__file__))

mapping = [
  [ "backend/*.cpp"                   , None              ],
  [ "backend/*.d"                     , None              ],
  [ "backend/*.h"                     , None              ],
  [ "backend/*.i"                     , None              ],
  [ "backend/*.l"                     , None              ],
  [ "backend/*.lo"                    , None              ],
  [ "backend/*.y"                     , None              ],
  [ "backend/*.yo"                    , None              ],
  [ "dbupdate/*.cpp"                  , None              ],
  [ "dbupdate/*.d"                    , None              ],
  [ "dbupdate/*.h"                    , None              ],
  [ "dbupdate/*.i"                    , None              ],
  [ "network/*.cpp"                   , None              ],
  [ "network/*.d"                     , None              ],
  [ "network/*.h"                     , None              ],
  [ "network/*.i"                     , None              ],
  [ "tools/*.cpp"                     , None              ],
  [ "tools/*.d"                       , None              ],
  [ "tools/*.h"                       , None              ],
  [ "tools/*.i"                       , None              ],
  [ "kernel/*.cpp"                    , None              ],
  [ "kernel/*.d"                      , None              ],
  [ "kernel/*.h"                      , None              ],
  [ "kernel/*.i"                      , None              ],
  [ "backend\VpOpenDialog_Not.h"      , "VpOpenDialog.h"  ],
  [ "kernel\VpFileMapping_Win32_1x.h" , "VpFileMapping.h" ],
  [ "kernel\VpSocket_Win32_1x.h"      , "VpSocket.h"      ],
  [ "kernel\Vp_Win32_1x.h"            , "Vp.h"            ],
]

sourceMap = {}
destMap   = {}

movesFile = ".windowsMoves.json"

def addMove(source, dest):
    if dest in destMap:
        log.warning(
            "destination collision at '" + dest + "': '" + source + "' and '" + destMap[dest]
        )
        del sourceMap[destMap[dest]]

    if source in sourceMap:
        log.warning(
            "source collision at '" + source + "': '" + dest + "' and '" + sourceMap[source]
        )
        del destMap[sourceMap[source]]

    sourceMap[source] = dest
    destMap[dest] = source

def buildMoves():
    for maps in mapping:
        srcglob = maps[0]
        dest = maps[1]

        moves = glob.glob(srcglob)

        if dest is not None:
            if len(moves) == 0:
                log.warning(
                    "destination '" + dest + "' has no source: '" + str(srcglob)
                )
            elif len(moves) == 1:
                source = moves[0]
                addMove(source, dest)
            else:
                raise RuntimeError(
                    "collision within a single glob: " + srcglob + str(moves)
                )
        else:
            if len(moves) == 0:
                log.warning(
                    "mapping '" + str(srcglob) + "' had no matches"
                )
            for move in moves:
                source = move
                dest = os.path.basename(source)
                addMove(source, dest)
          

def saveMoves(filename):
    with open(filename, "w") as fo:
        json.dump(destMap, fo)

def readMoves(filename):
    global sourceMap
    with open(filename) as fi:
        sourceMap = json.load(fi)

def doMoves(movesDict):
    for src, dest in movesDict.iteritems():
        shutil.move(src, dest)


def main(args):
    root = args.directory

    os.chdir(os.path.join(root, "src"))

    print os.path.exists(movesFile)

    if not os.path.exists(movesFile):
        print "Building File Moves"
        buildMoves()
        saveMoves(movesFile)
    else:
        readMoves(movesFile)
        os.remove(movesFile)

    print "Moving " + str(len(sourceMap)) + " files"
        
    doMoves(sourceMap)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="switchWindowsBuild.py moves vision source to a layout that builds in Visual Studio.  And back again"
    )
    parser.add_argument("directory", 
        help="directory to switch"
    )
   
    main(parser.parse_args())
