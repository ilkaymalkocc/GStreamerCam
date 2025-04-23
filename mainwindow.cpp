#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pipeline(nullptr)
    , appsink(nullptr)
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    gst_init(nullptr, nullptr);

    const char* pipelineStr = "udpsrc port=5000 ! tsparse ! tsdemux ! queue ! decodebin ! videoconvert ! video/x-raw,format=BGR ! appsink name=mysink";
    GError *error = nullptr;
    pipeline = gst_parse_launch(pipelineStr, &error);
    if (!pipeline || error) {
        qDebug() << "Hata:" << error->message;
        g_clear_error(&error);
        return;
    }

    appsink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    gst_app_sink_set_emit_signals((GstAppSink*)appsink, true);
    gst_app_sink_set_drop((GstAppSink*)appsink, true);
    gst_app_sink_set_max_buffers((GstAppSink*)appsink, 1);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(33); // ~30 fps
}

MainWindow::~MainWindow() {
    timer->stop();
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
    delete ui;
}

void MainWindow::updateFrame() {
    GstSample *sample = gst_app_sink_try_pull_sample((GstAppSink*)appsink, 10 * GST_MSECOND);
    if (!sample) return;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *s = gst_caps_get_structure(caps, 0);

    int width = 0, height = 0;
    const gchar *format = gst_structure_get_string(s, "format");
    gst_structure_get_int(s, "width", &width);
    gst_structure_get_int(s, "height", &height);

    qDebug() << "Gelen format:" << format;

    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        QImage image((const uchar *)map.data, width, height, QImage::Format_BGR888);
        if (!image.isNull()) {
            ui->label->setPixmap(QPixmap::fromImage(image).scaled(ui->label->size(), Qt::KeepAspectRatio));
        }
        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
}
