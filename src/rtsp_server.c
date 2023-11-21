/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

//#############################################################################################
//Modified from https://github.com/GStreamer/gst-rtsp-server/blob/master/examples/test-video.c
//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
//#############################################################################################

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gio/gio.h>

/* define this if you want the resource to only be available when using
 * user/password as the password */
#undef WITH_AUTH
#define WITH_AUTH 0


#undef WITH_OMXENC
#define WITH_OMXENC 1

/* this timeout is periodically run to clean up the expired sessions from the
 * pool. This needs to be run explicitly currently but might be done
 * automatically as part of the mainloop. */
static gboolean
timeout (GstRTSPServer * server)
{
    GstRTSPSessionPool *pool;

    pool = gst_rtsp_server_get_session_pool (server);
    gst_rtsp_session_pool_cleanup (pool);
    g_object_unref (pool);

    return TRUE;
}


int
main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
#if WITH_AUTH
    GstRTSPAuth *auth;
    GstRTSPToken *token;
    gchar *basic;
    GstRTSPPermissions *permissions;
#endif

    gst_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* create a server instance */
    server = gst_rtsp_server_new ();

    gst_rtsp_server_set_service(server,"8554"); //set the port #
#if WITH_AUTH
    /* make a new authentication manager. it can be added to control access to all
     * the factories on the server or on individual factories. */
    auth = gst_rtsp_auth_new ();

    /* make user token */
    token =
        gst_rtsp_token_new (GST_RTSP_TOKEN_MEDIA_FACTORY_ROLE, G_TYPE_STRING,
                            "user", NULL);
    basic = gst_rtsp_auth_make_basic ("user", "password");

    gst_rtsp_auth_add_basic (auth, basic, token);
    g_free (basic);
    gst_rtsp_token_unref (token);

    /* configure in the server */
    gst_rtsp_server_set_auth (server, auth);
#endif

    /* get the mount points for this server, every server has a default object
     * that be used to map uri mount points to media factories */
    mounts = gst_rtsp_server_get_mount_points (server);

    /* make a media factory for a test stream. The default media factory can use
     * gst-launch syntax to create pipelines.
     * any launch line works as long as it contains elements named pay%d. Each
     * element with pay%d names will be a stream */
    factory = gst_rtsp_media_factory_new ();
    

#if WITH_OMXENC
     gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=30/1 ! "
                                       "omxh264enc  bitrate=10000000 preset-level=1 ! video/x-h264,profile=baseline ! h264parse ! rtph264pay name=pay0 pt=96 " ")");
     
#else
     
     gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video0 ! video/x-raw,width=640,height=480,framerate=3/1 ! "
                                       "x264enc pass=qual quantizer=20 tune=zerolatency ! rtph264pay name=pay0 pt=96 " ")");
#endif

    /*
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "videotestsrc ! video/x-raw,width=352,height=288,framerate=15/1 ! "
                                       "x264enc ! rtph264pay name=pay0 pt=96 "
                                       "audiotestsrc ! audio/x-raw,rate=8000 ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
    */

   //To find microphone connected to webcam use the command line
   //See http://oz9aec.net/software/gstreamer/pulseaudio-device-names
   //pactl list | grep -A2 'Source #' | grep 'Name: .*\.usb' | cut -d" " -f2

   /*
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video2 ! videoconvert ! video/x-raw,width=640,height=480,framerate=30/1 ! "
                                       "x264enc pass=qual quantizer=20 tune=zerolatency ! rtph264pay name=pay0 pt=96 "
                                       "pulsesrc device=alsa_input.usb-046d_0825_1E32BDE0-02.analog-mono ! audio/x-raw ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
   */
    /*
    gst_rtsp_media_factory_set_launch (factory, "( "
                                       "v4l2src device=/dev/video2 ! videoconvert ! video/x-raw,width=640,height=480,framerate=30/1 ! "
                                       "x264enc pass=qual quantizer=20 tune=zerolatency ! rtph264pay name=pay0 pt=96 "
                                       "pulsesrc device=alsa_input.pci-0000_00_1f.3.analog-stereo ! audio/x-raw ! "
                                       "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
    */

#if WITH_AUTH
    /* add permissions for the user media role */
    permissions = gst_rtsp_permissions_new ();
    gst_rtsp_permissions_add_role (permissions, "user",
                                   GST_RTSP_PERM_MEDIA_FACTORY_ACCESS, G_TYPE_BOOLEAN, TRUE,
                                   GST_RTSP_PERM_MEDIA_FACTORY_CONSTRUCT, G_TYPE_BOOLEAN, TRUE, NULL);
    gst_rtsp_media_factory_set_permissions (factory, permissions);
    gst_rtsp_permissions_unref (permissions);
#endif

    /* attach the test factory to the /test url */
    gst_rtsp_mount_points_add_factory (mounts, "/test", factory);
    //gst_rtsp_mount_points_add_factory (mounts, "/test", factory);

    /* don't need the ref to the mapper anymore */
    g_object_unref (mounts);

    /* attach the server to the default maincontext */
    if (gst_rtsp_server_attach (server, NULL) == 0)
        goto failed;

    /* add a timeout for the session cleanup */
    g_timeout_add_seconds (2, (GSourceFunc) timeout, server);

    /* start serving, this never stops */
    g_main_loop_run (loop);

    return 0;

    /* ERRORS */
failed:
    {
        g_print ("failed to attach the server\n");
        return -1;
    }
}


