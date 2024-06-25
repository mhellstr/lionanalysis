#!/bin/bash
dumpfile=""
configfile=""
numblocks=1
sizeblock=""
numframes=""
startblock=1
filewithnumframes=""
run_total=0
olflags=""
minblock=""; explicitly_set_minblock=0
maxblock=""; explicitly_set_maxblock=0
show_help() {
    cat <<EOF
olblock.sh -h
    show help
olblock.sh -d <dumpfile> -c <configfile> -n <numblocks> [-N <numframes>]  [-X <file-with-numframes>]
olblock.sh -d <dumpfile> -c <configfile> -s <sizeblock> [-N <numframes>]  [-X <file-with-numframes>]
    -n : give the number of blocks (program finds out size of each block)
    -s : give the size of teach block (program finds out the number of blocks)
    -t : run total as well
    -N : specify the total number of frames in <dumpfile> (default is to find out, but can take some time for large dumpfiles)
    -X : the <file-with-numframes> contains a single word with the number of frames in <dumpfile>
EOF
    exit
}
while getopts ":hd:c:n:s:N:S:X:tom:M:" opt;
do
    case "$opt" in
        d) dumpfile="$OPTARG"; ;;
        c) configfile="$OPTARG"; ;;
        n) numblocks="$OPTARG"; ;;
        s) sizeblock="$OPTARG"; ;;
        t) run_total=1; ;;
        o) olflags="$olflags --overwrite"; ;;
        N) numframes="$OPTARG"; ;;  #set this on the command line if you already know it... otherwise grep -c is run below which is kind of slow and annoying
        #S) startblock="$OPTARG"; ;;
        X) filewithnumframes="$OPTARG"; ;;
        h) show_help; ;;
        m) minblock="$OPTARG"; explicitly_set_minblock=1; ;;
        M) maxblock="$OPTARG"; explicitly_set_maxblock=1; ;;
    esac
done
if [[ "$dumpfile" = "" ]]; then
    echo "Must specify dumpfile (-d)"
    exit 1
fi
if [[ ! -f "$dumpfile" ]] ;then
    echo "ERROR! Dumpfile $dumpfile does not exist."
    exit 1
fi
if [[ "$configfile" = "" ]]; then
    echo "Must specify config file (-c)"
    exit 1
fi
if [[ ! -f "$configfile" ]] ;then
    echo "ERROR! Config file $configfile does not exist."
    exit 1
fi

if [[ "$numframes" = "" ]]; then
    if [[ "$filewithnumframes" != "" ]]; then
        if [[ -f "$filewithnumframes" ]]; then
            numframes=$(cat $filewithnumframes)
            echo "Getting number of frames from $filewithnumframes"
        else
            echo "Couldn't find file-with-numframes $filewithnumframes ... exiting"
            exit 1
        fi
    else
        numframes=$(grep -c -F "XYZ
TIME" $dumpfile)
    fi
    echo "Found $numframes frames in $dumpfile"
else
    echo "Specified $numframes frames in $dumpfile"
fi

if [[ "$sizeblock" != "" ]]; then
    numblocks=$(( numframes/sizeblock ))
    modulo=$(( numframes % sizeblock ))
    #if [[ $modulo -eq 0 ]]; then
        #numblocks=$(( numblocks - 1 ))
    #fi
else
    sizeblock=$(( numframes / numblocks ))
    modulo=$(( numframes % sizeblock ))
fi

if [[ "$minblock" = "" ]]; then
    minblock=1
fi
if [[ "$maxblock" = "" ]]; then
    maxblock=$numblocks
fi

echo "#blocks: $numblocks ; #frames per block: $sizeblock (block $numblocks: $((sizeblock+$modulo)) ); I will start at block $minblock and stop at block $maxblock"
echo "olflags: $olflags"
#exit

startbyte=0
for ((i=$minblock; i<=$maxblock; i++))
do
    suffix=_${numblocks}_${i}
    basenamedumpfile=$(basename $dumpfile)
    basenameconfigfile=$(basename $configfile)
    mintimestep=0
    if [[ $explicitly_set_minblock -eq 1 && $i -eq $minblock ]]; then
        # if this is the first block that you read, you cannot use StartByte and the "simple" MaxTimeStep (simple means $sizeblock)
        # instead you must use MinTimeStep and the "complicated" maxTimeStep (complicated means $sizeblock*i)
        mintimestep=$(((i-1)*sizeblock + 1))
        maxtimestep=$(( i*sizeblock ))
    else
        maxtimestep=$sizeblock
        [[ $i -eq $numblocks ]] && maxtimestep=$((sizeblock+modulo))
    fi
    echo "AT BLOCK $i (size $maxtimestep); startbyte $startbyte"
    {
        echo "StartByte $startbyte"
        echo "DumpFile $dumpfile"
        echo "MinTimestep $mintimestep"
        [[ $i -lt $numblocks ]] && echo "MaxTimestep $maxtimestep"
        echo "Suffix $suffix"
        sed '/DumpFile\>/d; /Dumpfile\>/d; /MaxTimestep\>/d; /MaxTimeStep\>/d; /StartByte\>/d; /Suffix\>/d; ' $configfile
    } > ${basenameconfigfile}_${basenamedumpfile}${suffix}
    lionanalysis.exe $olflags -d $dumpfile ${basenameconfigfile}_${basenamedumpfile}${suffix} | tee olblockout_${basenameconfigfile}_${basenamedumpfile}${suffix}
    startbyte=$(awk '/Closing/ {print $NF}' olblockout_${basenameconfigfile}_${basenamedumpfile}${suffix} | tail -n 1)
    if [[ "$startbyte" = "" ]]; then
        echo "End of block $i : Don't know where I stopped reading the  dumpfile!!"
        exit
    fi
done

if [[ $run_total -eq 1 ]]; then
    suffix=_total
    basenamedumpfile=$(basename $dumpfile)
    basenameconfigfile=$(basename $configfile)
    echo "AT TOTAL"
    {
        echo "StartByte 0"
        echo "DumpFile $dumpfile"
        echo "Suffix $suffix"
        sed '/DumpFile\>/d; /Dumpfile\>/d; /MaxTimestep\>/d; /MaxTimeStep\>/d; /StartByte\>/d; /Suffix\>/d; ' $configfile
    } > ${basenameconfigfile}_${basenamedumpfile}${suffix}
    lionanalysis.exe $olflags -d $dumpfile ${basenameconfigfile}_${basenamedumpfile}${suffix} | tee olblockout_${basenameconfigfile}_${basenamedumpfile}${suffix}
fi
