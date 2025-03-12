#!/bin/bash

# rm -rf bin

# cd build
# rm -rf *
# cmake .. -DCMAKE_BUILD_TYPE=Release && make -j

# cd .. 

mkdir -p outputs

datasets=("FourPeople" "Johnny" "KristenAndSara")  # Add your dataset names here
enc_config=encoder_randomaccess_vtm.cfg
vid_config=FourPeople_mod.cfg
num_frames=600


for name in "${datasets[@]}"
do
    video=${name}_1280x720_60.yuv
    enc_config=encoder_randomaccess_vtm.cfg

    for qp in 32 22 27 37
    do
        echo "Encoding ${name} with QP ${qp}"
        ./bin/EncoderAppStatic --InputFile="./data/${video}" -c ./cfg/${enc_config} -c ./cfg/per-sequence/${vid_config} --QP="${qp}" --IntraPeriod="64" --BitstreamFile="./outputs/str_${name}_${qp}.bin" --ReconFile="./outputs/${name}_${qp}.yuv" --FramesToBeEncoded="${num_frames}" --SummaryOutFilename="./outputs/summary_${name}_${qp}" > outputs/${name}_${qp}.txt &
    done
    #wait

    enc_config=encoder_randomaccess_vtm_disable_bio.cfg
    for qp in 32 22 27 37
    do
        echo "Encoding ${name} with QP ${qp} and disable_bio"
        ./bin/EncoderAppStatic --InputFile="./data/${video}" -c ./cfg/${enc_config} -c ./cfg/per-sequence/${vid_config} --QP="${qp}" --IntraPeriod="64" --BitstreamFile="./outputs/str_${name}_${qp}_diable_bio.bin" --ReconFile="./outputs/${name}_${qp}_diable_bio.yuv" --FramesToBeEncoded="${num_frames}" --SummaryOutFilename="./outputs/summary_${name}_${qp}_diable_bio" > outputs/${name}_${qp}_diable_bio.txt &
    done
    wait
done

wait

# enc_config=encoder_randomaccess_vtm_disable_bio.cfg
# for qp in 32 #22 27 37
# do
#     ./bin/EncoderAppStatic --InputFile="./data/${video}" -c ./cfg/${enc_config} -c ./cfg/per-sequence/${vid_config} --QP="${qp}" --IntraPeriod="64" --BitstreamFile="./outputs/str_${name}_${qp}_diable_bio.bin" --ReconFile="./outputs/${name}_${qp}_diable_bio.yuv" --FramesToBeEncoded="${num_frames}" --SummaryOutFilename="./outputs/summary_${name}_${qp}_diable_bio" > outputs/${name}_${qp}_diable_bio.txt &
# done

# wait