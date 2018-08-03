#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

/*
실행 : sudo modprobe bcm2835-v4l2 를 먼저 치고
컴파일 : g++ -o vidio_record vidio_record.cpp $(pkg-config --libs --cflags opencv)
저장 파일 실행 : omxplayer test.mp4
*/

using namespace cv;
using namespace std;

#define VIDEO_FPS   30
#define BUFSIZE     150
#define DURATION    10
#define TEXT_POS    230, 30

#define TEXT_ON

int flag;
struct timeval UTCtime_r;

void *timer_func(void *data)
{
  int time;
  struct timeval UTCtime_s, UTCtime_e;

  gettimeofday(&UTCtime_s, NULL); // start time
  sleep(DURATION-1);
  usleep(999500);
  flag=0;

  while(!flag)
    {
      usleep(100);
      gettimeofday(&UTCtime_e, NULL); // end time

      if((UTCtime_e.tv_usec - UTCtime_s.tv_usec) < 0) // 자리수 맞추기 1sec 빼고 100000usec 더해주는 것
        {
          UTCtime_r.tv_sec = UTCtime_e.tv_sec - UTCtime_s.tv_sec-1;
          UTCtime_r.tv_usec = (1000000 + UTCtime_e.tv_usec) - UTCtime_s.tv_usec;
        }
      else
        {
          UTCtime_r.tv_sec = UTCtime_e.tv_sec - UTCtime_s.tv_sec;
          UTCtime_r.tv_usec = UTCtime_e.tv_usec - UTCtime_s.tv_usec;
        }
      if(UTCtime_r.tv_usec > 999000)
        flag = 1;
    }
}

int main(int argc, char *argv[])
{
  IplImage *frame = 0;
  CvVideoWriter *writer = 0;
  CvFont font;
  char buf[BUFSIZE];
  int run = 1;
  pthread_t p_thread;

  if(pthread_create(&p_thread, NULL, timer_func, NULL) < 0)
    {
      perror("eroor : thread_create");
      exit(2);
    }
  pthread_detach(p_thread);

  cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 1); // text를 만드는 함수에 대한 font 구조체를 초기화
  CvCapture *capture = cvCreateCameraCapture(0); // 카메라로부터 영상을 받아와 CvCapture 형태의 포인터로 저장

  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

  cvNamedWindow("video", CV_WINDOW_AUTOSIZE); //

  cvGrabFrame(capture); // 프레임 잡아내기
  frame = cvRetrieveFrame(capture); // 잡아낸 프레임 읽기

  int fps = VIDEO_FPS;

  strcpy(buf, argv[1]);

  writer = cvCreateVideoWriter(buf, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvGetSize(frame));

  int count = 0;
  time_t UTCtime;
  struct tm *tm;

  while(run)
    {
      cvGrabFrame(capture);
      frame = cvRetrieveFrame(capture);

      if(!frame || cvWaitKey(10) >= 0)
        {
          break;
        };
#ifdef TEXT_ON

      if((count%10) == 0)
        {
          time(&UTCtime);
          tm = localtime(&UTCtime);
          strftime(buf, sizeof(buf), "%Y-%m-%j %H:%M:%S", tm);
        }
      cvPutText(frame, buf, cvPoint(TEXT_POS), &font, CV_RGB(255, 255, 255));
#endif

      cvWriteFrame(writer, frame);
      cvShowImage("video", frame);

      if(flag)
        run = 0;
    }
  cvReleaseVideoWriter(&writer);
  cvReleaseCapture(&capture);
  cvDestroyWindow("video");

  return 0;
}
