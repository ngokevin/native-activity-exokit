#include <unistd.h>
// #include <stdio.h>
// #include <fcntl.h>
// #include <cstdlib>
// #include <cstring>
// #include <dlfcn.h>
// #include <errno.h>
#include <string>
#include <map>
#include <thread>
#include <v8.h>
// #include <exout>

using namespace v8;

namespace node {
  extern std::map<std::string, std::pair<void *, bool>> dlibs;
  int Start(int argc, char* argv[]);
}

// #include "build/libexokit/dlibs.h"

const char *LOG_TAG = "exokit";
constexpr ssize_t STDIO_BUF_SIZE = 64 * 1024;

void pumpStdout(int fd) {
  char buf[STDIO_BUF_SIZE + 1];
  ssize_t i = 0;
  ssize_t lineStart = 0;

  for (;;) {
    ssize_t size = read(fd, buf + i, sizeof(buf) - i);

    if (size > 0) {
      for (ssize_t j = i; j < i + size; j++) {
        if (buf[j] == '\n') {
          buf[j] = 0;
          // ML_LOG_TAG(Info, LOG_TAG, "%s", buf + lineStart);

          lineStart = j + 1;
        }
      }

      i += size;

      if (i >= sizeof(buf)) {
        ssize_t lineLength = i - lineStart;
        memcpy(buf, buf + lineStart, lineLength);
        i = lineLength;
        lineStart = 0;
      }
    } else {
      if (i > 0) {
        buf[i] = 0;
        // ML_LOG_TAG(Info, LOG_TAG, "%s", buf);
      }
      break;
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

    if (size > 0) {
      for (ssize_t j = i; j < i + size; j++) {
        if (buf[j] == '\n') {
          buf[j] = 0;
          // ML_LOG_TAG(Error, LOG_TAG, "%s", buf + lineStart);

          lineStart = j + 1;
        }
      }

      i += size;

      if (i >= sizeof(buf)) {
        ssize_t lineLength = i - lineStart;
        memcpy(buf, buf + lineStart, lineLength);
        i = lineLength;
        lineStart = 0;
      }
    } else {
      if (i > 0) {
        buf[i] = 0;
        // ML_LOG_TAG(Error, LOG_TAG, "%s", buf);
      }
      break;

    }
    return 0;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    return node::Start(argc, argv);
  }

  int stdoutfds[2];
  int stderrfds[2];
  pipe(stdoutfds);
  pipe(stderrfds);

  // if (stdoutfds[1] != 1) {
    close(1);
    dup2(stdoutfds[1], 1);
  // }
  // if (stderrfds[1] != 2) {
    close(2);
    dup2(stderrfds[1], 2);
  // }

  // read stdout/stderr in threads
  int stdoutReadFd = stdoutfds[0];
  std::thread stdoutReaderThread([stdoutReadFd]() -> void { pumpStdout(stdoutReadFd); });
  int stderrReadFd = stderrfds[0];
  std::thread stderrReaderThread([stderrReadFd]() -> void { pumpStderr(stderrReadFd); });

  // exout << "---------------------exokit start" << std::endl;

  std::atexit([]() -> void {
    // exout << "---------------------exokit end" << std::endl;
  });

  int result;
  const char *argsEnv = getenv("ARGS");
  if (argsEnv) {
    char args[4096];
    strncpy(args, argsEnv, sizeof(args));

    char *argv[64];
    size_t argc = 0;

    int argStartIndex = 0;
    for (int i = 0;; i++) {
      const char c = args[i];
      if (c == ' ') {
        args[i] = '\0';
        argv[argc] = args + argStartIndex;
        argc++;
        argStartIndex = i + 1;
        continue;
      } else if (c == '\0') {
        argv[argc] = args + argStartIndex;
        argc++;
        break;
      } else {
        continue;
      }
    }

    for (int i = 0; i < argc; i++) {
      // exout << "get arg " << i << ": " << argv[i] << std::endl;
    }



/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = AcquireASensorManagerInstance(state);
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(
                                        engine.sensorManager,
                                        ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(
                                    engine.sensorManager,
                                    state->looper, LOOPER_ID_USER,
                                    NULL, NULL);

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                                                       &event, 1) > 0) {
                        LOGI("accelerometer: x=%f y=%f z=%f",
                             event.acceleration.x, event.acceleration.y,
                             event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}
//END_INCLUDE(all)
