import re
import matplotlib.pyplot as plt
import bjontegaard as bd
import argparse
from bjontegaard_metric import BD_PSNR, BD_RATE

# Function to extract values from the text file
def extract_values(filename):
    with open(filename, 'r') as file:
        content = file.read()
    
    # Regular expression to match the required values
    match = re.search(r'600\s+a\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)', content)
    if match:
        bitrate = float(match.group(1))
        y_psnr = float(match.group(2))
        u_psnr = float(match.group(3))
        v_psnr = float(match.group(4))
        yuv_psnr = float(match.group(5))
        return bitrate, y_psnr, u_psnr, v_psnr, yuv_psnr
    else:
        raise ValueError("Values not found in the file")

def collect_rdpoints(filenames):
    bitrates = []
    y_psnrs = []
    u_psnrs = []
    v_psnrs = []
    yuv_psnrs = []

    for filename in filenames:
        bitrate, y_psnr, u_psnr, v_psnr, yuv_psnr = extract_values(filename)
        bitrates.append(bitrate)
        y_psnrs.append(y_psnr)
        u_psnrs.append(u_psnr)
        v_psnrs.append(v_psnr)
        yuv_psnrs.append(yuv_psnr)

    return bitrates, y_psnrs, u_psnrs, v_psnrs, yuv_psnrs


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate RD curve from text files.')
    parser.add_argument('--data', type=str, required=True, help='Name of the dataset')
    args = parser.parse_args()    
    
    
    # List of QP values and corresponding filenames
    qp_values = [22, 27, 32, 37]
    filenames = [f'/home/jaehong/VVCSoftware_VTM/outputs/{args.data}_{qp}.txt' for qp in qp_values]
    filenames_nobio = [f'/home/jaehong/VVCSoftware_VTM/outputs/{args.data}_{qp}_diable_bio.txt' for qp in qp_values]

    bitrates, _, _, _, psnrs = collect_rdpoints(filenames)
    bitrates_nobio, _, _, _, psnrs_nobio = collect_rdpoints(filenames_nobio)

    # bitrates_nobio = [bitrate + 100 for bitrate in bitrates_nobio]
    # psnrs_nobio = [psnr - 3 for psnr in psnrs_nobio]
    
    bd_rate = bd.bd_rate(bitrates, psnrs, bitrates_nobio, psnrs_nobio, method='akima')
    bd_psnr = bd.bd_psnr(bitrates, psnrs, bitrates_nobio, psnrs_nobio, method='akima')
     
    # bd_rate = BD_RATE(bitrates, psnrs, bitrates_nobio, psnrs_nobio)
    # bd_psnr = BD_PSNR(bitrates, psnrs, bitrates_nobio, psnrs_nobio)
    # bdrate > 0: the first configuration is better
    # bdrate < 0: the second configuration is better
    print(f"{args.data} | BD-rate: {bd_rate:.2f}%, BD-PSNR: {bd_psnr:.2f}dB")

    # Plot RD curve
    plt.figure()
    # plt.plot(bitrates, y_psnrs, 'ro-', label='Y-PSNR')
    # plt.plot(bitrates, u_psnrs, 'go-', label='U-PSNR')
    # plt.plot(bitrates, v_psnrs, 'bo-', label='V-PSNR')
    plt.plot(bitrates, psnrs, 'ko-', label='YUV-PSNR')
    plt.plot(bitrates_nobio, psnrs_nobio, 'ro-', label='YUV-PSNR (disable bio)')

    plt.xlabel('Bitrate (kbps)')
    plt.ylabel('PSNR (dB)')
    plt.title(f'{args.data} RD Curve')
    plt.legend()
    plt.grid(True)

    # Save the plot as an image
    plt.savefig(f'/home/jaehong/VVCSoftware_VTM/{args.data}_rd_curve.png')