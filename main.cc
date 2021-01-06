#include <assert.h>
#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

#include <string>

void Usage(int argc, char* argv[]) {
  printf("\n");
  printf("Usage: %s  input url/file\n", argv[0]);

  printf("Options:\n");
  printf("    -h  help\n");

  exit(1);
}

int main(int argc, char** argv) {
  std::string i_input_url;

  int c = -1;
  opterr = 0;
  while ((c = getopt(argc, argv, "hi:")) != -1) {
    switch (c) {
      case 'h':
        Usage(argc, argv);
        break;
      case '?':
        printf("Unknown option `-%c'.\n", optopt);
        Usage(argc, argv);
        break;
      default:
        Usage(argc, argv);
    }
  }

  if (optind >= argc) {
    printf("Invalid parameter, no input url/file\n");

    Usage(argc, argv);
  } else {
    i_input_url = argv[optind];

    if (i_input_url.empty()) {
      printf("Invalid parameter, no input url/file\n");

      Usage(argc, argv);
    }
  }

  // av_log_set_level(AV_LOG_DEBUG);

  int ret = 0;

  AVFormatContext* input_context = nullptr;
  AVStream* input_video_stream = nullptr;

  int input_video_streamIndex = -1;

  AVPacket pkt;
  uint32_t frames = 0;

  // open input
  input_context = avformat_alloc_context();
  ret = avformat_open_input(&input_context, i_input_url.c_str(), nullptr,
                            nullptr);
  if (ret != 0) {
    printf("Error opening input\n");

    goto end;
  }

  input_context->fps_probe_size = 0;
  input_context->max_ts_probe = 0;
  ret = avformat_find_stream_info(input_context, NULL);
  if (ret < 0) {
    printf("Error finding stream info\n");

    goto end;
  }

  av_dump_format(input_context, 0, i_input_url.c_str(), 0);

  // find video stream
  input_video_streamIndex =
      av_find_best_stream(input_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
  if (input_video_streamIndex < 0) {
    printf("Error, no video stream\n");

    goto end;
  } else {
    input_video_stream = input_context->streams[input_video_streamIndex];
  }

  while (1) {
    ret = av_read_frame(input_context, &pkt);
    if (ret < 0)
      break;

    if (input_video_stream->index != pkt.stream_index)
      continue;

    int64_t pts_ms = av_rescale_q_rnd(
        pkt.pts, input_video_stream->time_base, av_make_q(1, 1000),
        (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

    printf("Get video packet, is_key_frame %d, pts %ld (ms), size %d\n",
           (pkt.flags & AV_PKT_FLAG_KEY), pts_ms, pkt.size);
    av_packet_unref(&pkt);
    frames++;
  }

  printf("Total video frames, %d\n", frames);

end:
  if (input_context)
    avformat_close_input(&input_context);

  if (ret < 0 && ret != AVERROR_EOF) {
    char error_buffer[128] = {'\0'};
    av_strerror(ret, error_buffer, 128);
    fprintf(stderr, "Error occurred: %s\n", error_buffer);
    return 1;
  }

  return 0;
}
