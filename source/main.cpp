/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
    Tencent switcher GUI
    Copyright (C) 2021  CaiMiao
*/

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <string>

// #ifdef __SWITCH__
#include <switch.h>
// #endif

namespace i18n = brls::i18n; // for loadTranslations() and getStr()
using namespace i18n::literals; // for _i18n

Result rc;
u64 languageCode;
SetRegion regionCode;
bool bT;

const char *regionName[] = { "JPN", "USA", "EUR", "AUS", "HTK", "CHN" };

Result runtimeRc;
bool bHardcore = false;

// for lang selection item
int langItemSel(int i)
{
    rc = setGetLanguageCode(&languageCode);
    if (R_FAILED(rc))
        return i;
    SetLanguage lcid;
    rc = setMakeLanguage(languageCode, &lcid);
    if (R_FAILED(rc))
        return i;
    switch (lcid)
    {
    case SetLanguage_JA :
        return 0;
        break;
    case SetLanguage_ENUS :
        return 1;
        break;
    case SetLanguage_KO :
        return 2;
        break;
    case SetLanguage_ZHHANS :
        return 3;
        break;
    case SetLanguage_ZHHANT :
        return 4;
        break;
    default:
        break;
    }
    return i;
}

// show 1 button dialog only shows info message
void showMsgDlg(std::string msgStr, std::string btnStr, bool cancelable = false)
{
    brls::Dialog* dlg = new brls::Dialog(msgStr);

    brls::GenericEvent::Callback closeCallback = [dlg](brls::View* view) {
        dlg->close();
    };

    dlg->addButton(btnStr, closeCallback);

    dlg->setCancelable(cancelable);

    dlg->open();
}

void mainForm()
{
    rc = setGetLanguageCode(&languageCode);
    rc = setGetRegionCode(&regionCode);
    rc = setsysGetT(&bT);

    // Create a sample view
    brls::TabFrame* rootFrame = new brls::TabFrame();
    rootFrame->setTitle("main/name"_i18n);
    //rootFrame->setIcon(BOREALIS_ASSET("icon/rip.jpg"));

    // tab lists
    brls::List* mainTabList = new brls::List();
    brls::List* infoTabList = new brls::List();
    brls::List* brickTabList = new brls::List();


    brls::ListItem* safeTenItem = new brls::ListItem("main/ez/safeT"_i18n);
    safeTenItem->getClickEvent()->subscribe([=](brls::View* view) {
        setMakeLanguageCode(SetLanguage_ZHHANS, &languageCode);
        runtimeRc = setsysSetLanguageCode(languageCode);
        setGetLanguageCode(&languageCode);
        if (R_SUCCEEDED(runtimeRc))
            brls::Logger::info(fmt::format("Success. LanguageCode: {}", (char*)&languageCode));
        runtimeRc = setsysSetRegionCode(SetRegion_CHN);
        setGetRegionCode(&regionCode);
        if (R_SUCCEEDED(runtimeRc))
            brls::Logger::info(fmt::format("Success. RegionCode: {}", regionCode));
        runtimeRc = setsysSetT(true);
        setsysGetT(&bT);
        if (R_SUCCEEDED(runtimeRc))
            brls::Logger::info(fmt::format("Success. IsT: {}", bT));
        auto msg = fmt::format("{}\nLanguageCode: {}\nRegionCode: {}\nIsT: {}", "main/ez/ok_msg"_i18n, (char*)&languageCode, regionCode, bT);
        showMsgDlg(msg, "brls/hints/ok"_i18n);
        });

    brls::ListItem* safeNonTenItem = new brls::ListItem("main/ez/safeNT"_i18n);
    safeNonTenItem->getClickEvent()->subscribe([=](brls::View* view) {
        runtimeRc = setsysSetRegionCode(SetRegion_HTK);
        setGetRegionCode(&regionCode);
        if (R_SUCCEEDED(runtimeRc))
            brls::Logger::info(fmt::format("Success. RegionCode: {}", regionCode));
        runtimeRc = setsysSetT(false);
        setsysGetT(&bT);
        if (R_SUCCEEDED(runtimeRc))
            brls::Logger::info(fmt::format("Success. IsT: {}", bT));
        auto msg = fmt::format("{}\nLanguageCode: {}\nRegionCode: {}\nIsT: {}", "main/ez/ok_msg"_i18n, (char*)&languageCode, regionCode, bT);
        showMsgDlg(msg, "brls/hints/ok"_i18n);
    });

    brls::ListItem* setEnglishItem = new brls::ListItem("Set to English language", "", "when you are gaijin");
    setEnglishItem->getClickEvent()->subscribe([=](brls::View* view) {
        runtimeRc = setsysGetT(&bT);
        if (R_FAILED(rc) || bT == true) {
            auto msg = "You will brick yourself when IsT = true. Aborted.\n(Please use the second item first.)";
            showMsgDlg(msg, "brls/hints/ok"_i18n);
        } else {
            setMakeLanguageCode(SetLanguage_ENUS, &languageCode);
            runtimeRc = setsysSetLanguageCode(languageCode);
            setGetLanguageCode(&languageCode);
            if (R_SUCCEEDED(runtimeRc))
                brls::Logger::info(fmt::format("Success. LanguageCode: {}", (char*)&languageCode));
            auto msg = fmt::format("{}\nLanguageCode: {}\nRegionCode: {}\nIsT: {}", "main/ez/ok_msg"_i18n, (char*)&languageCode, regionCode, bT);
            showMsgDlg(msg, "brls/hints/ok"_i18n);
        }
    });

    {   // list 1
        mainTabList->addView(new brls::Header("main/ez/lbl"_i18n, false));

        mainTabList->addView(safeTenItem);
        mainTabList->addView(safeNonTenItem);
        mainTabList->addView(setEnglishItem);

        mainTabList->addView(new brls::Label(brls::LabelStyle::SMALL, "main/common/disc_msg"_i18n, true));

        rootFrame->addTab("main/tabs/ez"_i18n, mainTabList);
    }

    brls::Table* currentTable = new brls::Table();
    currentTable->addRow(brls::TableRowType::BODY, "main/terms/LanguageCode"_i18n, i18n::getCurrentLocale());
    currentTable->addRow(brls::TableRowType::BODY, "main/terms/RegionCode"_i18n, fmt::format("{} ({})",regionCode, regionName[(int)regionCode]));
    currentTable->addRow(brls::TableRowType::BODY, "main/terms/IsT"_i18n, fmt::format("{}", bT));

    brls::ListItem* showValueItem = new brls::ListItem("main/info/btn_show_val"_i18n);
    showValueItem->getClickEvent()->subscribe([=](brls::View* view) {
        auto msg = fmt::format("LanguageCode: {}\nRegionCode: {}\nIsT: {}", (char*)&languageCode, regionCode, bT);
        showMsgDlg(msg, "brls/hints/ok"_i18n);
    });

    brls::ListItem* aboutItem = new brls::ListItem("main/info/about"_i18n);
    aboutItem->getClickEvent()->subscribe([=](brls::View* view) {
        showMsgDlg("main/info/about_msg"_i18n, "brls/hints/ok"_i18n);
    });

    brls::ListItem* brickModeItem = new brls::ListItem("main/info/btn_brick"_i18n, "", "main/info/desc_brick"_i18n);
    brickModeItem->getClickEvent()->subscribe([=](brls::View* view) {
        if (!bHardcore)
        {
            bHardcore = true;
            brickModeItem->willDisappear();
            brickModeItem->hide([](){});
            brls::Application::giveFocus(aboutItem);
            rootFrame->addSeparator();
            rootFrame->addTab("main/tabs/hc"_i18n, brickTabList);
        }
        brls::Application::notify("main/info/brick_msg"_i18n);
    });

    {   // list 2
        infoTabList->addView(new brls::Header("main/info/now_val"_i18n, false));

        infoTabList->addView(currentTable);
        infoTabList->addView(showValueItem);
        infoTabList->addView(aboutItem);
        infoTabList->addView(brickModeItem);

        rootFrame->addTab("main/tabs/info"_i18n, infoTabList);
    }

    brls::ListItem* showValueItemDupe = new brls::ListItem("main/info/btn_show_val"_i18n);
    showValueItemDupe->getClickEvent()->subscribe([=](brls::View* view) {
        auto msg = fmt::format("LanguageCode: {}\nRegionCode: {}\nIsT: {}", (char*)&languageCode, regionCode, bT);
        showMsgDlg(msg, "brls/hints/ok"_i18n);
    });

    brls::SelectListItem* languageItem = new brls::SelectListItem(
        "main/terms/Language"_i18n,
        {
            "main/locale/ja"_i18n,
            "main/locale/en-US"_i18n,
            "main/locale/ko"_i18n,
            "main/locale/zh-Hans"_i18n,
            "main/locale/zh-Hant"_i18n,
        }, langItemSel(1));
    languageItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
        SetLanguage i;
        switch (selection)
        {
        case 0:
            i = SetLanguage_JA;
            break;
        case 1:
        default:
            i = SetLanguage_ENUS;
            break;
        case 2:
            i = SetLanguage_KO;
            break;
        case 3:
            i = SetLanguage_ZHHANS;
            break;
        case 4:
            i = SetLanguage_ZHHANT;
            break;
        }
        setMakeLanguageCode(i, &languageCode);
        runtimeRc = setsysSetLanguageCode(languageCode);
        setGetLanguageCode(&languageCode);
        auto msg = fmt::format("Success.\nLanguageCode: {}", (char*)&languageCode);
        if (R_SUCCEEDED(runtimeRc))
            brls::Application::notify(msg);
    });

    brls::SelectListItem* regionItem = new brls::SelectListItem(
        "main/terms/Region"_i18n,
        {
            "JPN (Japan)",
            "USA (The Americas)",
            "EUR (Europe)",
            "AUS (Australia/New Zealand)",
            "HTK (Hongkong/Taiwan/Korea)",
            "CHN (China)",
        }, (int)regionCode);
    regionItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
        SetRegion i;
        switch (selection)
        {
        case 0:
            i = SetRegion_JPN;
            break;
        case 1:
            i = SetRegion_USA;
            break;
        case 2:
            i = SetRegion_EUR;
            break;
        case 3:
            i = SetRegion_AUS;
            break;
        case 4:
        default:
            i = SetRegion_HTK;
            break;
        case 5:
            i = SetRegion_CHN;
            break;
        }
        runtimeRc = setsysSetRegionCode(i);
        setGetRegionCode(&regionCode);
        auto msg = fmt::format("Success.\nRegionCode: {}", regionCode);
        if (R_SUCCEEDED(runtimeRc))
            brls::Application::notify(msg);
    });

    brls::SelectListItem* isTenItem = new brls::SelectListItem(
        "main/terms/PlatformRegion"_i18n,
        {
            "main/terms/global"_i18n,
            "main/terms/tencent"_i18n,
        }, bT ? 1 : 0);
    isTenItem->getValueSelectedEvent()->subscribe([=](size_t selection) {
        bool b;
        switch (selection)
        {
        case 0:
        default:
            b = false;
            break;
        case 1:
            b = true;
            break;
        }
        runtimeRc = setsysSetT(b);
        setsysGetT(&bT);
        auto msg = fmt::format("Success.\nIsT: {}", bT);
        if (R_SUCCEEDED(runtimeRc))
            brls::Application::notify(msg);
    });

    //brls::InputListItem* fullkeyboardItem = new brls::CustomInputListItem("test swkbd input", "Klaatu barada nikto", "Enter passcode", "", 20, (int)(SwkbdType)SwkbdType_All);

    {   // list 3
        brickTabList->addView(new brls::Label(brls::LabelStyle::MEDIUM, "main/hc/warn_msg"_i18n, true));

        brickTabList->addView(showValueItemDupe);

        brickTabList->addView(new brls::Header("main/hc/lbl"_i18n, false));

        brickTabList->addView(languageItem);
        brickTabList->addView(regionItem);
        brickTabList->addView(isTenItem);

        // brickTabList->addView(new brls::Header("test item", false));
        // brickTabList->addView(fullkeyboardItem);
    }   // spawns on brickModeItem->getClickEvent()

    // Add the root view to the stack
    brls::Application::pushView(rootFrame);

    //brls::Application::giveFocus(safeNonTenItem);
}

int main(int argc, char* argv[])
{
    // Init the app
    brls::Logger::setLogLevel(brls::LogLevel::INFO);

    i18n::loadTranslations();
    if (!brls::Application::init("main/name"_i18n))
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    {
        brls::Dialog* dialog = new brls::Dialog("main/common/disc_dlg_msg"_i18n);

        brls::GenericEvent::Callback closeCallback = [dialog](brls::View* view) {
            dialog->close();
            mainForm();
        };

        std::string continueStr = "main/common/btn_idk"_i18n;

        dialog->addButton(continueStr, closeCallback);

        dialog->setCancelable(false);

        dialog->open();
    }

    // Run the app
    while (brls::Application::mainLoop())
        ;

    // Exit
    return EXIT_SUCCESS;
}
