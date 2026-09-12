from pydub import AudioSegment
import os

def MP32WAV(mp3_path, wav_path):
    """
    This function converts an MP3 file to a WAV file
    :param mp3_path: path of the MP3 file
    :param wav_path: path of the WAV file
    """
    MP3_File = AudioSegment.from_mp3(file=mp3_path).set_frame_rate(32000)
    MP3_File.export(wav_path, format="wav", bitrate='32k')
# MP32WAV('F:/mp3/3.mp3','F:/mp3/3.wav')

def run_main():

    # Paths of the MP3 files and WAV files
    path1 = 'F:\myesp\master\esp-idf\pro\s3_lvgl_v7\mp3'
    path2 = "F:\wav"
    paths = os.listdir(path1)
    mp3_paths = []
    # Get the paths of the MP3 files
    for mp3_path in paths:
        mp3_paths.append(path1 + "/" + mp3_path)
    # print(mp3_paths)

    # Get the WAV file paths corresponding to the MP3 files
    wav_paths = []
    for mp3_path in mp3_paths:
        print(mp3_path)
        wav_path = path2 + "/" + mp3_path[1:].split('.')[0].split('/')[-1] + '.wav'
        print(wav_path)
        wav_paths.append(wav_path)
    print(wav_paths)

    # Convert the MP3 files to WAV files
    for (mp3_path, wav_path) in zip(mp3_paths, wav_paths):

        # print(mp3_path)
        # print(mp3_paths)
        MP32WAV(mp3_path, wav_path)

if __name__ == '__main__':
    run_main()