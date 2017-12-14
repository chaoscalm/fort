import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "../../controls"
import com.fortfirewall 1.0

RowLayout {

    Layout.fillWidth: true

    readonly property alias label: label
    readonly property alias field: spinCombo.field
    readonly property alias combo: spinCombo.combo

    property alias names: spinCombo.names
    property alias values: spinCombo.values

    Label {
        id: label
        Layout.fillWidth: true
    }

    SpinCombo {
        id: spinCombo
        Layout.maximumWidth: implicitWidth

        fieldPreferredWidth: 140

        field {
            from: -1
            to: 9999
        }
    }
}
