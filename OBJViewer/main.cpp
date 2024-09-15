#include <QGuiApplication>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QDiffuseMapMaterial>

#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureImage>


#include <QFile>
#include <QTextStream>
#include <QMap>

QMap<QString, QString> parseMtlFile(const QString &mtlFilePath) {
    QMap<QString, QString> materialTextures;
    QFile file(mtlFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return materialTextures;

    QTextStream in(&file);
    QString currentMaterial;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("newmtl")) {
            currentMaterial = line.split(" ").last();
        } else if (line.startsWith("map_Kd") && !currentMaterial.isEmpty()) {
            QString textureFile = line.split(" ").last();
            materialTextures[currentMaterial] = textureFile;
        }
    }
    return materialTextures;
}

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
    Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

    // Set up camera
    Qt3DRender::QCamera* camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Set up camera controller
    Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLinearSpeed(-75.0f); // Reverse panning direction
    camController->setLookSpeed(180.0f);  //  Look direction
    camController->setCamera(camera);

    // Load OBJ file
    Qt3DRender::QMesh* mesh = new Qt3DRender::QMesh();
    //mesh->setSource(QUrl::fromLocalFile("C:\\Projects\\SVS_Data\\Bug reading .obj files\\Export_Gréoux_Cliff_Test_Sample\\20201006_Gréoux_Cliff_Small_Sample.obj"));
    //mesh->setSource(QUrl::fromLocalFile("C:\\Projects\\SVS_Data\\Objectfilesdata\\Obj_Non Mosaic\\DR6_photo_edited\\DR6_cut.obj"));
    mesh->setSource(QUrl::fromLocalFile("C:\\Projects\\SVS_Data\\Objectfilesdata\\Kilbaha_Mosaic\\Kilbaha.obj"));

    // Parse MTL file
    QMap<QString, QString> materialTextures = parseMtlFile("C:\\Projects\\SVS_Data\\Objectfilesdata\\Kilbaha_Mosaic\\Kilbaha.mtl");

    // Set up transform
    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform();
    transform->setScale(0.04f);
    transform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

    // Set up entity
    Qt3DCore::QEntity* entity = new Qt3DCore::QEntity(rootEntity);
    entity->addComponent(mesh);
    entity->addComponent(transform);

    // Set up material
    //Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    //material->setDiffuse(QColor(QRgb(0x928327)));

    // Set up materials with textures from MTL file
    int i=0;
    for (auto it = materialTextures.begin(); it != materialTextures.end() /*&& i < 1*/; ++it, ++i) {
        Qt3DRender::QTexture2D* texture = new Qt3DRender::QTexture2D();
        Qt3DRender::QTextureImage* textureImage = new Qt3DRender::QTextureImage();
        textureImage->setSource(QUrl::fromLocalFile("C:\\Projects\\SVS_Data\\Objectfilesdata\\Kilbaha_Mosaic\\" + it.value()));
        texture->addTextureImage(textureImage);

        //Qt3DExtras::QDiffuseMapMaterial* material = new Qt3DExtras::QDiffuseMapMaterial(rootEntity);
        //material->setDiffuse(texture);

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(QColor(QRgb(0x928327 + 100*i)));

        //entity->addComponent(material);

        // Create a new entity for each material
        Qt3DCore::QEntity* materialEntity = new Qt3DCore::QEntity(rootEntity);
        materialEntity->addComponent(mesh);
        materialEntity->addComponent(material);
        materialEntity->addComponent(transform);

    }


    view->setRootEntity(rootEntity);
    view->show();

    return app.exec();
}
