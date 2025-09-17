#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>


void list_devices(const char *device_type) {
    const AVInputFormat *input_format = NULL;
    AVOutputFormat *output_format = NULL;
    AVDeviceInfoList *dev_list = NULL;
    AVDeviceInfo **devices = 0; 
    AVDeviceInfo *dev = 0; 
    //AVMediaType *ptype ;

    // Initialize FFmpeg device library
    avdevice_register_all();

    if (device_type == NULL || strcmp(device_type, "audio") == 0) {
        printf("Listing Audio Devices:\n");
        // Enumerate audio input devices
        input_format = av_find_input_format("alsa"); // or "pulse" for PulseAudio
        if (input_format) {
            int ret = avdevice_list_input_sources(input_format, NULL, NULL, &dev_list);
            if (ret >= 0 && dev_list) {
                for (int i = 0; i < dev_list->nb_devices; i++) {
                    dev = dev_list->devices[i];
                    if(dev) {
                        char *name = dev->device_name;
                        char *device_description = dev->device_description;
                        //AVMediaType *ptype = devices[i]->media_types;
                        printf("Audio Device %d: %s, nb_media_types: %d, device_description: %s\n", 
                            i, name, dev->nb_media_types, device_description);
                    }
                }
                //av_device_list_free(&dev_list);
            } else {
                printf("No audio devices found.\n");
            }
        }
    }
    if (device_type == NULL || strcmp(device_type, "video") == 0) {
        printf("Listing Audio Devices:\n");
        // Enumerate audio input devices
        input_format = av_find_input_format("v4l2"); // or "x11grab" for X11
        if (input_format) {
            int ret = avdevice_list_input_sources(input_format, NULL, NULL, &dev_list);
            if (ret >= 0 && dev_list) {
                for (int i = 0; i < dev_list->nb_devices; i++) {
                    dev = dev_list->devices[i];
                    if(dev) {
                        char *name = dev->device_name;
                        char *device_description = dev->device_description;
                        //AVMediaType *ptype = devices[i]->media_types;
                        printf("Audio Device %d: %s, nb_media_types: %d, device_description: %s\n", 
                            i, name, dev->nb_media_types, device_description);
                    }
                }
                //av_device_list_free(&dev_list);
            } else {
                printf("No audio devices found.\n");
            }
        }
    }    
#if 0
    if (device_type == NULL || strcmp(device_type, "video") == 0) {
        printf("Listing Video Devices:\n");
        // Enumerate video input devices
        output_format = av_find_output_format("v4l2"); // or "x11grab" for X11
        if (output_format || 1) {
            int ret = av_find_input_format(output_format, NULL, NULL, &dev_list);
            if (ret >= 0 && dev_list) {
                for (int i = 0; i < dev_list->nb_devices; i++) {
                    if(!devices) {
                        printf("Audio Device %d: %s\n", i, devices[0]->device_name);
                    }
                }
                //av_device_list_free(&dev_list);
            } else {
                printf("No video devices found.\n");
            }
        }
    }
#endif
}

int main(int argc, char *argv[]) {
    // Initialize FFmpeg
    avdevice_register_all();

    // List audio devices
    list_devices("audio");

    // List video devices
    list_devices("video");

    return 0;
}

