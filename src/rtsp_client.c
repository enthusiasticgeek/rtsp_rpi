//Usage
//First run
// ./rtsp_server
//Then run
//./rtsp_client rtsps://127.0.0.1:8554/test

//Display RTSP streaming of video
//(c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>

#include <string.h>
#include <math.h>
#include <gst/gst.h>
#include <glib.h>
#include <gio/gio.h>

#undef WITH_AUTH
#define WITH_AUTH 0


static gboolean bus_call (GstBus *bus,GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("Stream Ends\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);
        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);
        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
    }
    return TRUE;
}


/* Dynamically link */
static void on_pad_added (GstElement *element, GstPad *pad, gpointer data){
    GstPad *sinkpad;
    GstPadLinkReturn ret;
    GstElement *decoder = (GstElement *) data;
    /* We can now link this pad with the rtsp-decoder sink pad */
    g_print ("Dynamic pad created, linking source/demuxer\n");
    sinkpad = gst_element_get_static_pad (decoder, "sink");

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked (sinkpad)) {
        g_print("*** We are already linked ***\n");
        gst_object_unref (sinkpad);
        return;
    } else {
        g_print("proceeding to linking ...\n");
    }
    ret = gst_pad_link (pad, sinkpad);

    if (GST_PAD_LINK_FAILED (ret)) {
        //failed
        g_print("failed to link dynamically\n");
    } else {
        //pass
        g_print("dynamically link successful\n");
    }

    gst_object_unref (sinkpad);
}

/* Dynamically link */
static void on_pad_removed (GstElement *element, GstPad *pad, gpointer data){
    GstPad *sinkpad;
    GstPadLinkReturn ret;
    GstElement *decoder = (GstElement *) data;
    /* We can now link this pad with the rtsp-decoder sink pad */
    g_print ("Dynamic pad created, unlinking source/demuxer\n");
    sinkpad = gst_element_get_static_pad (decoder, "sink");

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked (sinkpad)) {
        g_print("proceeding to unlinking ...\n");
    } else {
        g_print("*** We are already unlinked ***\n");
        gst_object_unref (sinkpad);
        return;
    }
    ret = gst_pad_unlink (pad, sinkpad);

    if (GST_PAD_LINK_FAILED (ret)) {
        //failed
        g_print("failed to unlink dynamically\n");
    } else {
        //pass
        g_print("dynamically unlink successful\n");
    }
    gst_object_unref (sinkpad);
}


int main (int argc, char *argv[])
{
   
    if(argc!=2){
      g_print("Please pass the URL of the RTSP stream\ne.g. %s rtsps://127.0.0.1:8554/test\n",argv[0]);
      exit(1);
    }

    GMainLoop *loop;
    GstBus *bus;
    GstElement *pipeline;
    GstElement *rtspsrc;
    GstElement *rtph264depay;
    GstElement *h264parse;
    GstElement *avdec_h264;
    GstElement *videoqueue0;
    GstElement *videoconvert;
    GstElement *rtppcmadepay;
    GstElement *alawdec;
    GstElement *audioqueue0;
    GstElement *audioconvert;

    GstElement *video_sink; /* The video sink element which receives XOverlay commands */
    GstElement *audio_sink;

    /*config struct*/

    /* Initializing GStreamer */
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    //gst-launch-0.10 rtspsrc location=rtsp://<ip> ! decodebin ! ffmpegvideoconvert ! autovideosink
    //gst-launch -v rtspsrc location="rtsp://<ip> ! rtpmp4vdepay ! mpeg4videoparse ! ffdec_mpeg4 ! ffmpegvideoconvert! autovideosink
    //gst-launch -v rtspsrc location="rtsp://<ip> ! rtpmp4vdepay ! ffdec_mpeg4 ! ffmpegvideoconvert! autovideosink
    /* Create Pipe's Elements */
    pipeline = gst_pipeline_new ("video player");
    g_assert (pipeline);

    rtspsrc = gst_element_factory_make ("rtspsrc", "rtspsrc0");
    g_assert (rtspsrc);


    //Video elements
    rtph264depay = gst_element_factory_make ("rtph264depay", "rtph264depay0");
    g_assert (rtph264depay);
    h264parse = gst_element_factory_make ("h264parse", "h264parse0");
    g_assert (h264parse);
    avdec_h264 = gst_element_factory_make ("decodebin", "avdec_h2640");
    g_assert (avdec_h264);
    videoqueue0 = gst_element_factory_make ("queue", "videoqueue0");
    g_assert (videoqueue0);
    videoconvert = gst_element_factory_make ("videoconvert", "videoconvert0");
    g_assert (videoconvert);
    //Video Sink
    //video_sink = gst_element_factory_make ("autovideosink", "autovideosink0");
    video_sink = gst_element_factory_make ("ximagesink", "autovideosink0");
    g_assert (video_sink);
    //video
    g_object_set (G_OBJECT (video_sink), "sync", FALSE, NULL);
    //g_object_set (G_OBJECT (video_sink), "async-handling", TRUE, NULL);


    /*
    //Audio Elements
    rtppcmadepay = gst_element_factory_make ("rtppcmadepay",  "rtppcmadepay0");
    g_assert(rtppcmadepay);
    alawdec = gst_element_factory_make ("decodebin", "alawdec0");
    g_assert (alawdec);
    audioqueue0 = gst_element_factory_make ("queue", "audioqueue0");
    g_assert (audioqueue0);
    audioconvert = gst_element_factory_make ("audioconvert", "audioconvert0");
    g_assert (audioconvert);
    //Audio Sink
    audio_sink = gst_element_factory_make ("autoaudiosink", "autoaudiosink0");
    g_assert (audio_sink);
    //audio
    g_object_set (G_OBJECT (audio_sink), "sync", FALSE, NULL);
    //g_object_set (G_OBJECT (audio_sink), "async-handling", TRUE, NULL);
    */


    /* Set video Source */
    g_object_set (G_OBJECT (rtspsrc), "location", argv[1], NULL);
    g_object_set (G_OBJECT (rtspsrc), "do-rtcp", TRUE, NULL);
    g_object_set (G_OBJECT (rtspsrc), "latency", 0, NULL);

#if WITH_AUTH
    g_object_set (G_OBJECT (rtspsrc), "user-id", "user", NULL);
    g_object_set (G_OBJECT (rtspsrc), "user-pw", "password", NULL);
#endif
    /* Putting a Message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);




    //Make sure: Every elements was created ok
    if (!pipeline || !rtspsrc || !rtph264depay || !h264parse || !avdec_h264 || !videoqueue0 || !videoconvert || !video_sink) {
        g_printerr ("One of the elements wasn't created... Exiting\n");
        return -1;
    }

    // Add Elements to the Bin
    gst_bin_add_many (GST_BIN (pipeline), rtspsrc, rtph264depay, h264parse, avdec_h264, videoqueue0, videoconvert, video_sink, NULL);

    // Link confirmation
    if (!gst_element_link_many (rtph264depay, h264parse, avdec_h264, NULL)){
        g_warning ("Linking part (A)-1 Fail...");
        return -2;
    }
    // Link confirmation
    if (!gst_element_link_many (videoqueue0, videoconvert, video_sink, NULL)){
        g_warning ("Linking part (A)-2 Fail...");
        return -4;
    }
    // Dynamic Pad Creation
    if(! g_signal_connect (rtspsrc, "pad-added", G_CALLBACK (on_pad_added),rtph264depay))
    {
        g_warning ("Linking part (1) with part (A)-1 Fail...");
    }

    // Dynamic Pad Creation
    if(! g_signal_connect (avdec_h264, "pad-added", G_CALLBACK (on_pad_added),videoqueue0))
    {
        g_warning ("Linking part (2) with part (A)-2 Fail...");
    }

    /* Run the pipeline */
    g_print ("Playing: %s\n", argv[1]);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    g_main_loop_run (loop);

    /* Ending Playback */
    g_print ("End of the Streaming... ending the playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);

    /* Eliminating Pipeline */
    g_print ("Eliminating Pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));

    return 0;
}
