#ifndef WIDGE_COMMAND_H
#define WIDGE_COMMAND_H
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cstring>
#include "pugixml.hpp"
#include "a661_writer.h"
#include "a661_consts.h"

uint8_t parseEnum(const std::string& value) {
    if (value == "A661_TRUE") return A661_TRUE;
    else if (value == "A661_FALSE") return A661_FALSE;
    else if (value == "A661_ABSENT") return A661_ABSENT;
    else if (value == "A661_LEFT") return A661_LEFT;
    else if (value == "A661_RIGHT") return A661_RIGHT;
    else if (value == "A661_OPEN_DOWN") return A661_OPEN_DOWN;
    else if (value == "A661_CENTER") return A661_CENTER;
    else if (value == "A661_TOP") return A661_TOP;
    else if (value == "A661_TRUE_WITH_VALIDATION") return A661_TRUE_WITH_VALIDATION;
    else if (value == "A661_EDB_CHANGE_CONFIRMED") return A661_EDB_CHANGE_CONFIRMED;
    else if (value == "A661_SIZE_LEFT_TO_RIGHT") return A661_SIZE_LEFT_TO_RIGHT;
    else if (value == "A661_UNSELECTED") return A661_UNSELECTED;
    else if (value == "A661_OPEN_DOWN") return A661_OPEN_DOWN;
    else if (value == "A661_WRAP_BOTH") return A661_WRAP_BOTH;
}

class A661WidgeCommand {
public:
    explicit A661WidgeCommand(int id, int pId) : widgeId(id), parentId(pId) {}

    uint16_t widgeId;
    uint16_t parentId;

    virtual ~A661WidgeCommand() = default;
    virtual void serialize(std::ofstream& outFile) const = 0;
    virtual size_t size() const = 0;
    virtual uint16_t getType() const = 0;
    uint16_t getId() { return widgeId; }
};

class TabbedPanelGroupCommand : public A661WidgeCommand {
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t activeTabbedPanelId;
    uint8_t tabPosition;
    uint8_t automaticInsetSizeFlag;

public:
    TabbedPanelGroupCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy, uint16_t ss, uint16_t atpId, uint8_t tp, uint8_t aisf)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss),
            activeTabbedPanelId(atpId), tabPosition(tp), automaticInsetSizeFlag(aisf){}

    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, activeTabbedPanelId);
        writeUint8(outFile, tabPosition);
        writeUint8(outFile, automaticInsetSizeFlag);
        writePadding(outFile, 2);
    }

    size_t size() const override {
        return 32; 
    }

    uint16_t getType() const override { return A661_TABBED_PANEL_GROUP ; }  
};

//
class TabbedPanelCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t maxStringLength;
    uint16_t pictureReference;
    uint8_t picturePosition;
    uint8_t automicFocusMotion;
    uint8_t alignment;
    uint32_t insetSize;
    std::string labelString;
    uint32_t alignStrSz;
public:
    TabbedPanelCommand(int id, int parentId, uint8_t e, uint8_t v, uint16_t ss, uint16_t nfw, uint16_t mStrLen, uint16_t pRef, uint8_t pPos, uint8_t afm,
        uint8_t ali, uint32_t iS, std::string lstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), styleSet(ss), nextFocusedWidget(nfw), maxStringLength(mStrLen), pictureReference(pRef),
        picturePosition(pPos), automicFocusMotion(afm), alignment(ali), insetSize(iS), labelString(lstr)
    {
        uint32_t strSz = labelString.size() + 1;
        alignStrSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, maxStringLength);
        writeUint16(outFile, pictureReference);
        writeUint8(outFile, picturePosition);
        writeUint8(outFile, automicFocusMotion);
        writeUint8(outFile, alignment);
        writePadding(outFile, 1);
        writeUint32(outFile, insetSize);
        writeA661String(outFile, labelString, this->alignStrSz);
    }

    size_t size() const override {
        return 24 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_TABBED_PANEL; }  
};

//
class GpRectangleCommand : public A661WidgeCommand {
private:
    uint8_t anonymous;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint8_t colorIndex;
    uint8_t filled;
    uint8_t fillIndex;
    uint8_t halo;
public:
    GpRectangleCommand(int id, int parentId, uint8_t an, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy, uint16_t ss, uint16_t ci, 
        uint8_t bf, uint8_t fi, uint8_t h)
        : A661WidgeCommand(id, parentId), anonymous(an), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss),
        colorIndex(ci), filled(bf), fillIndex(fi), halo(h) {}
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, anonymous);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint8(outFile, colorIndex);
        writeUint8(outFile, filled);
        writeUint8(outFile, fillIndex);
        writeUint8(outFile, halo);
        writePadding(outFile, 2);
    }

    size_t size() const override {
        return 32;
    }

    uint16_t getType() const override { return A661_GP_RECTANGLE; }
};

// Label
class LabelCommand : public A661WidgeCommand {
private:
    uint8_t anonymous;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    int32_t rotationAngle;
    uint16_t styleSet;
    uint16_t maxStringLength;
    uint8_t motionAllowed;
    uint8_t font;
    uint8_t colorIndex;
    uint8_t alignment;
    std::string labelString;
    uint32_t alignStrSz;
public:
    LabelCommand(int id, int parentId, uint8_t an, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy, int32_t ra, uint16_t ss, 
        uint16_t maxStrLen, uint8_t ma, uint8_t f, uint16_t ci, uint8_t ali, std::string lstr)
        : A661WidgeCommand(id, parentId), anonymous(an), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), rotationAngle(ra), styleSet(ss),
        maxStringLength(maxStrLen), motionAllowed(ma), font(f), colorIndex(ci), alignment(ali) , labelString(lstr)
    {
        uint32_t strSz = labelString.size() + 1;
        alignStrSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, anonymous);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeInt32(outFile, rotationAngle);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, maxStringLength);
        writeUint8(outFile, motionAllowed);
        writeUint8(outFile, font);
        writeUint8(outFile, colorIndex);
        writeUint8(outFile, alignment);
        writeA661String(outFile, labelString, this->alignStrSz);
    }

    size_t size() const override {
        return 36 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_LABEL; }
};

//ComboBox
class ComboBoxCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint32_t selectingAreaWidth;
    uint32_t selectingAreaHeight;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t maxNumberOfEntries;
    uint16_t numberOfEntries;
    uint16_t selectedEntry;
    uint16_t maxStringLength;
    uint16_t openingEntry;
    uint8_t alignment;
    uint8_t openingMode;
    uint8_t automicFocusMotion;
    std::vector<std::string> entryList;
    
    uint32_t strTotalSz;
public:
    ComboBoxCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy, 
        uint32_t saW, uint32_t saH, uint16_t ss, uint16_t nfw, uint16_t maxNum, uint16_t nOfEntries, uint16_t se,
        uint16_t maxStrLen, uint16_t oe, uint8_t ali, uint8_t om, uint8_t afm)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), selectingAreaWidth(saW), 
        selectingAreaHeight(saH), styleSet(ss), nextFocusedWidget(nfw), maxNumberOfEntries(maxNum),  numberOfEntries(nOfEntries), selectedEntry(se),
        maxStringLength(maxStrLen), openingEntry(oe), alignment(ali), openingMode(om), automicFocusMotion(afm), strTotalSz(0)
    {
    }
   
    void addEntry(std::string e) {
        entryList.emplace_back(e);
        strTotalSz += (e.size() + 1);
    }
    
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint32(outFile, selectingAreaWidth);
        writeUint32(outFile, selectingAreaHeight);
        writeUint16(outFile, styleSet);

        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, maxNumberOfEntries);
        writeUint16(outFile, numberOfEntries);
        writeUint16(outFile, selectedEntry);
        writeUint16(outFile, maxStringLength);
        writeUint16(outFile, openingEntry);

        writeUint8(outFile, alignment);
        writeUint8(outFile, openingMode);
        writeUint8(outFile, automicFocusMotion);
        writePadding(outFile, 3);
        for (auto entryStr : entryList) {
            uint32_t sz = entryStr.size() + 1;
            writeA661String(outFile, entryStr, sz);
        }
        uint32_t alignStrSz = strTotalSz % 4 == 0 ? strTotalSz : 4 * (strTotalSz / 4 + 1);
        writePadding(outFile, alignStrSz - strTotalSz);
    }

    size_t size() const override {
        size_t size = 0;
        uint32_t alignStrSz = strTotalSz % 4 == 0 ? strTotalSz : 4 * (strTotalSz / 4 + 1);
        return 52 + alignStrSz;
    }

    uint16_t getType() const override { return A661_COMBO_BOX; }
};

//PicturePushButton
class PicturePushButtonCommand : public A661WidgeCommand{
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t pictureReference;
    uint16_t maxStringLength;
    uint8_t picturePosition;
    uint8_t automicFocusMotion;
    uint8_t alignment;
    std::string labelString;
    uint32_t alignStrSz;
public:
    PicturePushButtonCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t ss, uint16_t nfw, uint16_t pRef, uint16_t mStrLen, uint8_t pPos, uint8_t afm,
        uint8_t ali,  std::string lstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw), 
         pictureReference(pRef), maxStringLength(mStrLen),picturePosition(pPos), automicFocusMotion(afm), alignment(ali), labelString(lstr)
    {
        uint32_t strSz = labelString.size()+1;
        alignStrSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, pictureReference);
        writeUint16(outFile, maxStringLength);
        writeUint8(outFile, picturePosition);
        writeUint8(outFile, automicFocusMotion);
        writeUint8(outFile, alignment);
        writePadding(outFile, 1);
        writeA661String(outFile, labelString, this->alignStrSz);
    }

    size_t size() const override {
        return 36 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_PICTURE_PUSH_BUTTON; }
};

//EditBoxText
class EditBoxTextCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t startCursorPos;
    uint16_t maxStringLength;
    uint8_t automicFocusMotion;
    uint8_t reportAllChanges;
    uint8_t alignment;
    std::string labelString;
    uint32_t alignStrSz;
public:
    EditBoxTextCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t ss, uint16_t nfw, uint16_t scPos, uint16_t mStrLen, uint8_t pPos, uint8_t afm, uint8_t report,
        uint8_t ali, std::string lstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw),
        startCursorPos(scPos), maxStringLength(mStrLen), automicFocusMotion(afm), reportAllChanges(report), alignment(ali), labelString(lstr)
    {
        uint32_t strSz = labelString.size() + 1;
        alignStrSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, startCursorPos);
        writeUint16(outFile, maxStringLength);
        writeUint8(outFile, automicFocusMotion);
        writeUint8(outFile, reportAllChanges);
        writeUint8(outFile, alignment);
        writePadding(outFile, 1);
        writeA661String(outFile, labelString, this->alignStrSz);
    }

    size_t size() const override {
        return 36 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_EDIT_BOX_TEXT; }
};

//PushButton
class PushButtonCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t maxStringLength;
    uint8_t automicFocusMotion;
    uint8_t alignment;
    std::string labelString;
    uint32_t alignStrSz;
public:
    PushButtonCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t ss, uint16_t nfw, uint16_t mStrLen, uint8_t pPos, uint8_t afm,
        uint8_t ali, std::string lstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw),
         maxStringLength(mStrLen), automicFocusMotion(afm),  alignment(ali), labelString(lstr)
    {
        uint32_t strSz = labelString.size() + 1;
        alignStrSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, maxStringLength);
        writeUint8(outFile, automicFocusMotion);
        writeUint8(outFile, alignment);
        writeA661String(outFile, labelString, this->alignStrSz);
    }

    size_t size() const override {
        return 32 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_PUSH_BUTTON; }
};

//PopUpPanel
class PopUpPanelCommand : public A661WidgeCommand {
private:
    uint8_t uaPositionFlag;
    uint8_t automaticClosure;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
public:
    PopUpPanelCommand(int id, int parentId, uint8_t uaPosFlag, uint8_t ac, int32_t px, int32_t py, uint32_t sx, uint32_t sy,uint16_t ss)
        : A661WidgeCommand(id, parentId), uaPositionFlag(uaPosFlag), automaticClosure(ac), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss)
    {
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, uaPositionFlag);
        writeUint8(outFile, automaticClosure);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writePadding(outFile, 2);
    }

    size_t size() const override {
        return 28;
    }

    uint16_t getType() const override { return A661_POP_UP_PANEL; }
};
//ScrollList
class ScrollListCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t maxNumberOfEntries;
    uint16_t numberOfEntries;
    uint16_t selectedEntry;
    uint16_t maxStringLength;
    uint16_t firstAcessibleEntry;
    uint16_t firstVisibleEntry;
    uint8_t verticalScroll;
    uint8_t alignment;
    uint8_t flagReportVisibleEntry;
    uint8_t automicFocusMotion;

    std::vector<uint8_t> defaultStyleTextVec;
    std::vector<uint8_t> enableArray;
    std::vector<std::string> labelStringArray;

    uint32_t strTotalSz;
public:
    ScrollListCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
         uint16_t ss, uint16_t nfw, uint16_t maxNum, uint16_t nOfEntries, uint16_t se,
        uint16_t maxStrLen, uint16_t oe, uint16_t fv, uint8_t vs, uint8_t ali, uint8_t om, uint8_t afm)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw), 
        maxNumberOfEntries(maxNum), numberOfEntries(nOfEntries), selectedEntry(se),
        maxStringLength(maxStrLen), firstAcessibleEntry(oe), firstVisibleEntry(fv), verticalScroll(vs), alignment(ali), flagReportVisibleEntry(om), automicFocusMotion(afm), strTotalSz(0)
    {
    }

    void addLabelStringArray(std::string e) {
        labelStringArray.emplace_back(e);
        strTotalSz += (e.size() + 1);
    }

    void addEnableArray(uint8_t e) {
        enableArray.emplace_back(e);
        strTotalSz += 1;
    }

    void addDefaultStyleTextVec(uint8_t e) {
        defaultStyleTextVec.emplace_back(e);
    }

    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);

        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, maxNumberOfEntries);
        writeUint16(outFile, numberOfEntries);
        writeUint16(outFile, selectedEntry);
        writeUint16(outFile, maxStringLength);
        writeUint16(outFile, firstAcessibleEntry);
        writeUint16(outFile, firstVisibleEntry);

        writeUint8(outFile, verticalScroll);
        writeUint8(outFile, alignment);
        writeUint8(outFile, flagReportVisibleEntry);
        writeUint8(outFile, automicFocusMotion);
        for (auto defaultStyleText : defaultStyleTextVec) {
            writeUint8(outFile, defaultStyleText);
        }
        writePadding(outFile, 12 - defaultStyleTextVec.size());

        for (auto enable : enableArray) {
            writeUint8(outFile, enable);
        }

        for (auto labelString : labelStringArray) {
            uint32_t sz = labelString.size() + 1;
            writeA661String(outFile, labelString, sz);
        }
        uint32_t alignStrSz = strTotalSz % 4 == 0 ? strTotalSz : 4 * (strTotalSz / 4 + 1);
        writePadding(outFile, alignStrSz - strTotalSz);
    }

    size_t size() const override {
        size_t size = 0;
        uint32_t alignStrSz = strTotalSz % 4 == 0 ? strTotalSz : 4 * (strTotalSz / 4 + 1);
        return 56 + alignStrSz;
    }

    uint16_t getType() const override { return A661_SCROLL_LIST; }
};
//SizeToFitContainer
class SizeToFitContainerCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t numberOfVisibleChildren;
    uint8_t sizeToFitMode;
    uint32_t itemSpacing;
public:
    SizeToFitContainerCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t num,  uint8_t size,uint32_t itemSp)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), numberOfVisibleChildren(num), 
        sizeToFitMode(size), itemSpacing(itemSp){}
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, numberOfVisibleChildren);
        writeUint8(outFile, sizeToFitMode);
        writePadding(outFile, 1);
        writeUint32(outFile, itemSpacing);
    }

    size_t size() const override {
        return 32;
    }

    uint16_t getType() const override { return A661_SIZE_TO_FIT_CONTAINER; }
};
//ToggleButton
class ToggleButtonCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    uint16_t maxStringLength;
    uint8_t toggleState;
    uint8_t alternateFlag;
    uint8_t automicFocusMotion;
    uint8_t alignment;
    std::string labelString;
    std::string alternateLabelString;
    uint32_t alignStrSz;
public:
    ToggleButtonCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t ss, uint16_t nfw, uint16_t mStrLen, uint8_t tState, uint8_t aFlag, uint8_t afm,
        uint8_t ali, std::string lstr, std::string alstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw),
         maxStringLength(mStrLen), toggleState(tState), alternateFlag(aFlag), automicFocusMotion(afm), alignment(ali), labelString(lstr), alternateLabelString(alstr)
    {
        uint32_t totalstrSz = labelString.size() + alternateLabelString.size() + 2;
        alignStrSz = totalstrSz % 4 == 0 ? totalstrSz : 4 * (totalstrSz / 4 + 1);
    }
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeUint16(outFile, maxStringLength);
        writeUint8(outFile, toggleState);
        writeUint8(outFile, alternateFlag);
        writeUint8(outFile, automicFocusMotion);
        writeUint8(outFile, alignment);
        writePadding(outFile, 2);
        writeA661String(outFile, labelString, labelString.size() + 1);
        writeA661String(outFile, labelString, alternateLabelString.size() + 1);
        writePadding(outFile, this->alignStrSz - labelString.size() - alternateLabelString.size() - 2);
    }

    size_t size() const override {
        return 36 + this->alignStrSz;
    }

    uint16_t getType() const override { return A661_TOGGLE_BUTTON; }
};
//BasicContainer
class BasicContainerCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
public:
    BasicContainerCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py){}
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
    }

    size_t size() const override {
        return 16;
    }

    uint16_t getType() const override { return A661_BASIC_CONTAINER; }
};
//Symbol
class SymbolCommand : public A661WidgeCommand {
private:
    uint8_t motionAllowed;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    int32_t rotationAngle;
    uint16_t styleSet;
    uint16_t symbolReference;
    uint8_t colorIndex;
public:
    SymbolCommand(int id, int parentId, uint8_t an, uint8_t v, int32_t px, int32_t py, int32_t ra, uint16_t ss, uint16_t symbolRef, uint8_t ci)
        : A661WidgeCommand(id, parentId), motionAllowed(an), visible(v), posX(px), posY(py), rotationAngle(ra), styleSet(ss), symbolReference(symbolRef),colorIndex(ci) {}
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, motionAllowed);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeInt32(outFile, rotationAngle);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, symbolReference);
        writeUint8(outFile, colorIndex);
        writePadding(outFile, 3);
    }

    size_t size() const override {
        return 28;
    }

    uint16_t getType() const override { return A661_SYMBOL; }
};
//PopUpMenuButton
class PopUpMenuButtonCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint16_t styleSet;
    uint16_t nextFocusedWidget;
    int32_t popupPosX;
    int32_t popupPosY;
    uint32_t popupSizeX;
    uint32_t popupSizeY;
    uint16_t maxStringLength;
    uint16_t maxStringLengthPopUp;
    uint16_t pictureReference;
    uint8_t numberOfEntries;
    uint8_t picturePosition;
    uint8_t openingMode;
    uint8_t alignment;
    uint8_t automicFocusMotion;
    std::string labelString;

    std::vector<uint16_t> popUpIdentArray;
    std::vector<uint16_t> pictureArray;
    std::vector<uint8_t> enableArray;
    std::vector<std::string> stringArray;
    uint32_t alignSz;
    uint32_t labelstrAlignSz;
public:
    PopUpMenuButtonCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy,
        uint16_t ss, uint16_t nfw, int32_t ppX, int32_t ppY, uint32_t ppSizeX, uint32_t ppSizeY, uint16_t maxStrLen,uint16_t maxStrLenPopUp, uint16_t pr,
        uint8_t numE, uint8_t pp, uint8_t om, uint8_t ali, uint8_t afm, std::string lstr)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), styleSet(ss), nextFocusedWidget(nfw), popupPosX(ppX), popupPosY(ppY),
        popupSizeX(ppSizeX), popupSizeY(ppSizeY),maxStringLength(maxStrLen), maxStringLengthPopUp(maxStrLenPopUp), pictureReference(pr),
        numberOfEntries(numE), picturePosition(pp), openingMode(om), alignment(ali), automicFocusMotion(afm), labelString(lstr), alignSz(0)
    {
        uint32_t strSz = labelString.size() + 1;
        labelstrAlignSz = strSz % 4 == 0 ? strSz : 4 * (strSz / 4 + 1);
    }

    void addStringArray(std::string s) {
        stringArray.emplace_back(s);
        alignSz += (s.size() + 1);
    }

    void addEnableArray(uint8_t e) {
        enableArray.emplace_back(e);
        alignSz += 1;
    }

    void addPopUpIdentArray(uint16_t value) {
        popUpIdentArray.emplace_back(value);
    }

    void addPictureArray(uint16_t value) {
        pictureArray.emplace_back(value);
    }

    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint16(outFile, styleSet);
        writeUint16(outFile, nextFocusedWidget);
        writeInt32(outFile, popupPosX);
        writeInt32(outFile, popupPosY);
        writeUint32(outFile, popupSizeX);
        writeUint32(outFile, popupSizeY);
        writeUint16(outFile, maxStringLength);
        writeUint16(outFile, maxStringLengthPopUp);
        writeUint16(outFile, pictureReference);
        writeUint8(outFile, numberOfEntries);
        writeUint8(outFile, picturePosition);
        writeUint8(outFile, openingMode);
        writeUint8(outFile, alignment);
        writeUint8(outFile, automicFocusMotion);
        writePadding(outFile, 1);  //56

        writeA661String(outFile, labelString, labelstrAlignSz);

        for (auto defaultStyleText : popUpIdentArray) {
            writeUint16(outFile, defaultStyleText);
        }
        for (auto picture : pictureArray) {
            writeUint16(outFile, picture);
        }
        for (auto enable : enableArray) {
            writeUint8(outFile, enable);
        }
        for (auto str : stringArray) {
            uint32_t sz = str.size() + 1;
            writeA661String(outFile, str, sz);
        }
        uint32_t totalSz = alignSz % 4 == 0 ? alignSz : 4 * (alignSz / 4 + 1);
        writePadding(outFile, totalSz - alignSz);
    }

    size_t size() const override {
        uint32_t alignStrSz = alignSz % 4 == 0 ? alignSz : 4 * (alignSz / 4 + 1);
        return 56 + alignStrSz + labelstrAlignSz + popUpIdentArray.size()*2 + pictureArray.size()*2;
    }

    uint16_t getType() const override { return A661_POP_UP_MENU_BUTTON; }
};

//PagingContainer
class PagingContainerCommand : public A661WidgeCommand {
private:
    uint8_t enable;
    uint8_t visible;
    int32_t posX;
    int32_t posY;
    uint32_t sizeX;
    uint32_t sizeY;
    uint8_t wrappingType;
    uint8_t pagingControlPosition;
    uint16_t finePageDelta;
    uint16_t coarsePageDelta;
    uint16_t visibleChild;
    uint8_t reportVisibleChild;
    uint16_t styleSet;
    uint8_t anonymous;

public:
    PagingContainerCommand(int id, int parentId, uint8_t e, uint8_t v, int32_t px, int32_t py, uint32_t sx, uint32_t sy, uint8_t wt, uint8_t pp, uint16_t fPageDelta
        , uint16_t cpageDelta, uint16_t vc, uint8_t rvc,uint16_t ss, uint8_t an)
        : A661WidgeCommand(id, parentId), enable(e), visible(v), posX(px), posY(py), sizeX(sx), sizeY(sy), wrappingType(wt), pagingControlPosition(pp),
        finePageDelta(fPageDelta), coarsePageDelta(cpageDelta), visibleChild(vc), reportVisibleChild(rvc), styleSet(ss), anonymous(an){}
    void serialize(std::ofstream& outFile) const override {
        writeUint16(outFile, this->getType());
        writeUint16(outFile, widgeId);
        writeUint16(outFile, parentId);
        writeUint8(outFile, enable);
        writeUint8(outFile, visible);
        writeInt32(outFile, posX);
        writeInt32(outFile, posY);
        writeUint32(outFile, sizeX);
        writeUint32(outFile, sizeY);
        writeUint8(outFile, wrappingType);
        writeUint8(outFile, pagingControlPosition);
        writeUint16(outFile, finePageDelta);
        writeUint16(outFile, coarsePageDelta);
        writeUint16(outFile, visibleChild);
        writeUint8(outFile, reportVisibleChild);
        writeUint16(outFile, styleSet);
        writeUint8(outFile, anonymous);
    }

    size_t size() const override {
        return 36;
    }

    uint16_t getType() const override { return A661_PAGING_CONTAINER; }
};


static std::shared_ptr<A661WidgeCommand> createWidgeCommandFromXML(const pugi::xml_node& commandNode, uint16_t parentId = 0) {
    std::string type = commandNode.attribute("type").as_string();
    if (type == "TabbedPanelGroup") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t activeTabbedPanelId = commandNode.child("model").find_child_by_attribute("intprop", "name", "ActiveTabbedPanelID").text().as_int();
        uint8_t tabPosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "TabPosition").text().as_string());
        uint8_t automaticInsetSizeFlag = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticInsetSizeFlag").text().as_string());
        return std::make_shared<TabbedPanelGroupCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, styleSet, activeTabbedPanelId, tabPosition, automaticInsetSizeFlag);
    }
    else if (type == "TabbedPanel") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint16_t pictureReference = commandNode.child("model").find_child_by_attribute("intprop", "name", "PictureReference").text().as_int();
        uint8_t picturePosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PicturePosition").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        uint32_t insetSize = commandNode.child("model").find_child_by_attribute("intprop", "name", "InsetSize").text().as_int();
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        return std::make_shared<TabbedPanelCommand>(widgeId, parentId, enable, visible, styleSet, nextFocusedWidget, maxStringLength, pictureReference,
            picturePosition, automaticFocusMotion, alignment, insetSize, labelString);
    }
    else if (type == "GpRectangle") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t anonymous = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Anonymous").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint8_t colorIndex = commandNode.child("model").find_child_by_attribute("intprop", "name", "ColorIndex").text().as_int();
        uint8_t filled = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Filled").text().as_string());
        uint8_t fillIndex = commandNode.child("model").find_child_by_attribute("intprop", "name", "FillIndex").text().as_int();
        uint8_t halo = parseEnum(commandNode.child("model").find_child_by_attribute("intprop", "name", "Halo").text().as_string());
        return std::make_shared<GpRectangleCommand>(widgeId, parentId, anonymous, visible, posX, posY, sizeX, sizeY, styleSet, colorIndex, filled, fillIndex, halo);
    }
    else if (type == "Label") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t anonymous = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Anonymous").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        int32_t rotationAngle = commandNode.child("model").find_child_by_attribute("intprop", "name", "RotationAngle").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint8_t motionAllowed = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "MotionAllowed").text().as_string());
        uint8_t font = commandNode.child("model").find_child_by_attribute("intprop", "name", "Font").text().as_int();
        uint8_t colorIndex = commandNode.child("model").find_child_by_attribute("intprop", "name", "ColorIndex").text().as_int();
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        return std::make_shared<LabelCommand>(widgeId, parentId, anonymous, visible, posX, posY, sizeX, sizeY, rotationAngle, 
            styleSet, maxStringLength, motionAllowed, font, colorIndex, alignment, labelString);
    }
    else if (type == "ComboBox") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint32_t selectingAreaWidth = commandNode.child("model").find_child_by_attribute("intprop", "name", "SelectingAreaWidth").text().as_int();
        uint32_t selectingAreaHeight = commandNode.child("model").find_child_by_attribute("intprop", "name", "SelectingAreaHeight").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t maxNumberOfEntries = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxNumberOfEntries").text().as_int();
        uint16_t numberOfEntries = commandNode.child("model").find_child_by_attribute("intprop", "name", "NumberOfEntries").text().as_int();
        uint16_t selectedEntry = commandNode.child("model").find_child_by_attribute("intprop", "name", "SelectedEntry").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint16_t openingEntry = commandNode.child("model").find_child_by_attribute("intprop", "name", "OpeningEntry").text().as_int();
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        uint8_t openingMode = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "OpeningMode").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());

        auto comboBoxCmd = std::make_shared<ComboBoxCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, selectingAreaWidth, selectingAreaHeight,
            styleSet, nextFocusedWidget, maxNumberOfEntries, numberOfEntries, selectedEntry, maxStringLength, openingEntry, alignment, openingMode, automaticFocusMotion);
        for (auto entryNode : commandNode.child("model").child("arrayprop").children("stringprop")) {
            std::string value = entryNode.text().as_string();
            comboBoxCmd->addEntry(value);
        }
        return comboBoxCmd;
    }
    else if (type == "PicturePushButton") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t pictureReference = commandNode.child("model").find_child_by_attribute("intprop", "name", "PictureReference").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint8_t picturePosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PicturePosition").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        return std::make_shared<PicturePushButtonCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, styleSet, nextFocusedWidget, pictureReference,
            maxStringLength, picturePosition, automaticFocusMotion, alignment, labelString);
    }
    else if (type == "EditBoxText") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t startCursorPos = commandNode.child("model").find_child_by_attribute("intprop", "name", "StartCursorPos").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint8_t picturePosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PicturePosition").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        uint8_t reportAllChanges = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "ReportAllChanges").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        return std::make_shared<EditBoxTextCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, styleSet, nextFocusedWidget, startCursorPos,
            maxStringLength, picturePosition, automaticFocusMotion, reportAllChanges, alignment, labelString);
    }
    else if (type == "PushButton") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint8_t picturePosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PicturePosition").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        return std::make_shared<PushButtonCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, styleSet, nextFocusedWidget,
            maxStringLength, picturePosition, automaticFocusMotion, alignment, labelString);
    }
    else if (type == "PopUpPanel") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t uaPositionFlag = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "UAPositionFlag").text().as_string());
        uint8_t automaticClosure = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticClosure").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        return std::make_shared<PopUpPanelCommand>(widgeId, parentId, uaPositionFlag, automaticClosure, posX, posY, sizeX, sizeY, styleSet);
    }
    else if (type == "ScrollList") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t maxNumberOfEntries = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxNumberOfEntries").text().as_int();
        uint16_t numberOfEntries = commandNode.child("model").find_child_by_attribute("intprop", "name", "NumberOfEntries").text().as_int();
        uint16_t selectedEntry = commandNode.child("model").find_child_by_attribute("intprop", "name", "SelectedEntry").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint16_t firstAccessibleEntry = commandNode.child("model").find_child_by_attribute("intprop", "name", "FirstAccessibleEntry").text().as_int();
        uint16_t firstVisibleEntry = commandNode.child("model").find_child_by_attribute("intprop", "name", "FirstVisibleEntry").text().as_int();
        uint8_t verticalScroll = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "VerticalScroll").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        uint8_t flagReportVisibleEntry = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "FlagReportVisibleEntry").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());

        auto scrollListCmd = std::make_shared<ScrollListCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY,
            styleSet, nextFocusedWidget, maxNumberOfEntries, numberOfEntries, selectedEntry, maxStringLength, firstAccessibleEntry, firstVisibleEntry,
            verticalScroll,alignment, flagReportVisibleEntry, automaticFocusMotion);

        for (auto arraypropNode : commandNode.child("model").children("arrayprop")) {
            for (auto entry : arraypropNode.children()) {
                std::string value = entry.text().as_string();
                if (std::string(entry.name()) == "intprop") {
                    scrollListCmd->addDefaultStyleTextVec(static_cast<uint8_t>(std::stoi(value)));
                }
                else if (std::string(entry.name()) == "enumprop") {
                    scrollListCmd->addEnableArray(parseEnum(value));
                }
                else if (std::string(entry.name()) == "stringprop") {
                    scrollListCmd->addLabelStringArray(value);
                }
            }
        }
        return scrollListCmd;
    }
    else if (type == "SizeToFitContainer") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t numberOfVisibleChildren = commandNode.child("model").find_child_by_attribute("intprop", "name", "NumberOfVisibleChildren").text().as_int();
        uint8_t sizeToFitMode = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "SizeToFitMode").text().as_string());
        uint32_t itemSpacing = commandNode.child("model").find_child_by_attribute("intprop", "name", "ItemSpacing").text().as_int();

        return std::make_shared<SizeToFitContainerCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, numberOfVisibleChildren, sizeToFitMode, itemSpacing);
    }
    else if (type == "ToggleButton") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint8_t toggleState = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "ToggleState").text().as_string());
        uint8_t alternateFlag = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AlternateFlag").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();
        std::string alternateLabelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "AlternateLabelString").text().as_string();
        return std::make_shared<ToggleButtonCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, styleSet, nextFocusedWidget,
            maxStringLength, toggleState, alternateFlag, automaticFocusMotion, alignment, labelString, alternateLabelString);
    }
    else if (type == "BasicContainer") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        return std::make_shared<BasicContainerCommand>(widgeId, parentId, enable, visible, posX, posY);
    }
    else if (type == "Symbol") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t motionAllowed = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "MotionAllowed").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        int32_t rotationAngle = commandNode.child("model").find_child_by_attribute("intprop", "name", "RotationAngle").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t symbolReference = commandNode.child("model").find_child_by_attribute("intprop", "name", "SymbolReference").text().as_int();
        uint8_t colorIndex = commandNode.child("model").find_child_by_attribute("intprop", "name", "ColorIndex").text().as_int();
        return std::make_shared<SymbolCommand>(widgeId, parentId, motionAllowed, visible, posX, posY, rotationAngle, styleSet, symbolReference, colorIndex);
    }
    else if (type == "PopUpMenuButton") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint16_t nextFocusedWidget = commandNode.child("model").find_child_by_attribute("intprop", "name", "NextFocusedWidget").text().as_int();
        int32_t popupPosX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PopupPosX").text().as_int();
        int32_t popupPosY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PopupPosY").text().as_int();
        uint32_t popupSizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PopupSizeX").text().as_int();
        uint32_t popupSizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PopupSizeY").text().as_int();
        uint16_t maxStringLength = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLength").text().as_int();
        uint16_t maxStringLengthPopUp = commandNode.child("model").find_child_by_attribute("intprop", "name", "MaxStringLengthPopUp").text().as_int();
        uint16_t pictureReference = commandNode.child("model").find_child_by_attribute("intprop", "name", "PictureReference").text().as_int();
        uint8_t numberOfEntries = commandNode.child("model").find_child_by_attribute("intprop", "name", "NumberOfEntries").text().as_int();
        uint8_t picturePosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PicturePosition").text().as_string());
        uint8_t openingMode = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "OpeningMode").text().as_string());
        uint8_t alignment = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Alignment").text().as_string());
        uint8_t automaticFocusMotion = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "AutomaticFocusMotion").text().as_string());
        std::string labelString = commandNode.child("model").find_child_by_attribute("stringprop", "name", "LabelString").text().as_string();

        auto popUpMenuButtonCmd = std::make_shared<PopUpMenuButtonCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY,styleSet, nextFocusedWidget,
            popupPosX, popupPosY, popupSizeX, popupSizeY, maxStringLength, maxStringLengthPopUp, pictureReference, numberOfEntries, picturePosition,
            openingMode, alignment, automaticFocusMotion, labelString);

        auto popUpIdentArray = commandNode.child("model").find_child_by_attribute("arrayprop", "name", "PopUpIdentArray");
        for (auto entry : popUpIdentArray.children("intprop")) {
            uint16_t value = entry.text().as_uint();
            popUpMenuButtonCmd->addPopUpIdentArray(value);
        }
        auto pictureArray = commandNode.child("model").find_child_by_attribute("arrayprop", "name", "PictureArray");
        for (auto entry : pictureArray.children("intprop")) {
            uint16_t value = entry.text().as_uint();
            popUpMenuButtonCmd->addPictureArray(value);
        }
        auto enableArray = commandNode.child("model").find_child_by_attribute("arrayprop", "name", "EnableArray");
        for (auto entry : enableArray.children("enumprop")) {
            uint8_t value = parseEnum(entry.text().as_string());
            popUpMenuButtonCmd->addEnableArray(value);
        }
        auto stringArray = commandNode.child("model").find_child_by_attribute("arrayprop", "name", "StringArray");
        for (auto entry : stringArray.children("stringprop")) {
            std::string value = entry.text().as_string();
            popUpMenuButtonCmd->addStringArray(value);
        }
        return popUpMenuButtonCmd;
    }
    else if (type == "PagingContainer") {
        uint16_t widgeId = commandNode.child("model").find_child_by_attribute("intprop", "name", "WidgetIdent").text().as_int();
        uint8_t enable = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Enable").text().as_string());
        uint8_t visible = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Visible").text().as_string());
        int32_t posX = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosX").text().as_int();
        int32_t posY = commandNode.child("model").find_child_by_attribute("intprop", "name", "PosY").text().as_int();
        uint32_t sizeX = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeX").text().as_int();
        uint32_t sizeY = commandNode.child("model").find_child_by_attribute("intprop", "name", "SizeY").text().as_int();
        uint8_t wrappingType = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "WrappingType").text().as_string());
        uint8_t pagingControlPosition = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "PagingControlPosition").text().as_string());
        uint16_t finePageDelta = commandNode.child("model").find_child_by_attribute("intprop", "name", "FinePageDelta").text().as_int();
        uint16_t coarsePageDelta = commandNode.child("model").find_child_by_attribute("intprop", "name", "CoarsePageDelta").text().as_int();
        uint16_t visibleChild = commandNode.child("model").find_child_by_attribute("intprop", "name", "VisibleChild").text().as_int();
        uint8_t reportVisibleChild = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "ReportVisibleChild").text().as_string());
        uint16_t styleSet = commandNode.child("model").find_child_by_attribute("intprop", "name", "StyleSet").text().as_int();
        uint8_t anonymous = parseEnum(commandNode.child("model").find_child_by_attribute("enumprop", "name", "Anonymous").text().as_string());
        return std::make_shared<PagingContainerCommand>(widgeId, parentId, enable, visible, posX, posY, sizeX, sizeY, wrappingType, pagingControlPosition,
            finePageDelta, coarsePageDelta, visibleChild, reportVisibleChild, styleSet, anonymous);
    }
    return nullptr;
    //throw std::runtime_error("Unknown Command Type: " + type);
}
#endif
