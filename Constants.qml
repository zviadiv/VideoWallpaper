import QtQuick 2.2

QtObject {

    id: constants

    enum VideoFillMode
    {
        Cover = 0, Contain, Stretch
    }

    enum ScreenMode
    {
        Unique = 0, Shared, Copy
    }

    enum OverlayType
    {
        None = 0, Black, White
    }
}
