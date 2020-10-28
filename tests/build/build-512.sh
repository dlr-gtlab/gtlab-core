cp features/gitlab_pipeline_local_settings.pri local_settings.pri

/opt/Qt/5.12.5/gcc_64/bin/qmake $PROFILENAME -spec linux-g++ CONFIG+=$BUILDMODE #CONFIG+=qml_debug

make clean

make -j 20
