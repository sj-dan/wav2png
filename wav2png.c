#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include <math.h>
#include <sndfile.h>
#include <string.h>

void wav2png(const char *output_filename, int width, SNDFILE *sndfile, SF_INFO sinfo) {
   FILE *fp = fopen(output_filename, "wb");
   if (!fp)
      abort();
   png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   png_infop pnginfo = png_create_info_struct(png);
   if (setjmp(png_jmpbuf(png)))
      abort();
   png_init_io(png, fp);

   double numSamples = (double)sinfo.frames*sinfo.channels;

   width *= sinfo.channels;

   printf("Printing %.0f samples into PNG file:\n", numSamples);

   printf("Width: %d\n", width);

   int height = ceil(numSamples / width);

   png_set_IHDR(
       png,
       pnginfo,
       width, height,
       8,
       PNG_COLOR_TYPE_RGB,
       PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT,
       PNG_FILTER_TYPE_DEFAULT);
   png_write_info(png, pnginfo);

   uint32_t *samples = (uint32_t *)malloc(numSamples * sizeof(uint32_t));
   uint8_t *normalizedSamples = (uint8_t *)malloc(3 * width * sizeof(uint8_t));
   sf_count_t frames_read;

   for (int y = 0; y < height; y++)
   {
      frames_read = sf_readf_int(sndfile, samples, width/sinfo.channels);
      for (int i = 0; i < width; i++) {
         int pixel_num = y*width+i;
         if (pixel_num < sinfo.frames*sinfo.channels) {
            uint32_t sample = samples[i];
            normalizedSamples[3*i] = (uint8_t)(sample >> 24 & 0xFF);
            normalizedSamples[3*i+1] = (uint8_t)(sample >> 16 & 0xFF);
            normalizedSamples[3*i+2] = (uint8_t)(sample >> 8 & 0xFF);
         } else {
            normalizedSamples[3*i] = 0;
            normalizedSamples[3*i+1] = 0;
            normalizedSamples[3*i+2] = 0;
         }
      }
      png_write_row(png,normalizedSamples);
   }

   // png_write_image(png,samples);

   // End write
   png_write_end(png, NULL);
   if (png && pnginfo)
      png_destroy_write_struct(&png, &pnginfo);
   if (fp)
      fclose(fp);
   if (samples)
      free(samples);
   if (normalizedSamples)
      free(normalizedSamples);
}

void png2wav(const char *input_filename, const char *output_filename, int width_factor) {
   FILE *fp = fopen(input_filename, "rb");
   png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   png_infop pnginfo = png_create_info_struct(png);
   png_init_io(png, fp);
   png_read_info(png, pnginfo);
   int width, height, bitdepth, colorType;
   png_get_IHDR(png, pnginfo, &width, &height, &bitdepth, &colorType, NULL, NULL, NULL);
   printf("Width: %d, Height: %d\n",width,height);
   printf("Encoding %d pixels into WAV file:\n", (width*height));

   SF_INFO sinfo;
   if (width%width_factor==0)
      sinfo.channels = (int)(width/width_factor);  // Stereo audio
   else
      sinfo.channels = 1;
   printf("%d",sinfo.channels);
   sinfo.samplerate = 44100;  // Sample rate (adjust as needed)
   sinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
   SNDFILE *sndfile = sf_open(output_filename, SFM_WRITE, &sinfo);

   png_bytep row = (png_bytep)malloc(png_get_rowbytes(png, pnginfo));

   int *samplerow = (int *)malloc(width * sizeof(int));

   int coeff;
   if (colorType == PNG_COLOR_TYPE_RGBA)
      coeff = 4;
   else
      coeff = 3;

   for (int y = 0; y < height; y++) {
      png_read_row(png, row, NULL);

      for (int x = 0; x < width; x++) {
         int rv = row[coeff*x];
         int gv = row[coeff*x+1];
         int bv = row[coeff*x+2];
         samplerow[x] = (rv << 24) | (gv << 16) | (bv << 8);
      }
      sf_writef_int(sndfile, samplerow, width/sinfo.channels);
   }

   sf_close(sndfile);
   free(row);
   free(samplerow);
   png_destroy_read_struct(&png, &pnginfo, NULL);
   fclose(fp);
   return;
}

int main(int argc, char *argv[])
{
   int mode;
   int width = 2000;
   const char *input_filename;
   const char *output_filename;

   if ((argc == 4 || argc == 5) && strcmp(argv[1],"--reverse")==0) {
      input_filename = argv[2];
      output_filename = argv[3];
      mode = 2;
      if (argc == 5)
         width = atoi(argv[4]);
   } else if (argc == 3 || argc == 4) {
      input_filename = argv[1];
      output_filename = argv[2];
      mode = 1;
      if (argc == 4)
         width = atoi(argv[3]);
   } else {
      fprintf(stderr, "Usage: %s <input_wav_file> <output_png_file>\nOR\nUsage: %s --reverse <input_png_file> <output_wav_file>\n", argv[0], argv[0]);
      return 1;
   }

   if (mode == 1) {
      printf("Running in wav2png mode. (Mode 1)\n\n");
      SF_INFO info;
      SNDFILE *sndfile = sf_open(input_filename, SFM_READ, &info);
      if (!sndfile) {
         fprintf(stderr, "Error opening file: %s\n", sf_strerror(NULL));
         return 1;
      }

      if ((info.format & SF_FORMAT_PCM_24) == info.format)
      {
         fprintf(stderr, "The input file is not a supported 24-bit PCM WAV file.\n");
         sf_close(sndfile);
         return 1;
      }

      wav2png(output_filename, width, sndfile, info);

      sf_close(sndfile);
   } else {
      printf("Running in png2wav mode. (Mode 2)\n\n");
      png2wav(input_filename,output_filename,width);
   }

   return 0;
}
