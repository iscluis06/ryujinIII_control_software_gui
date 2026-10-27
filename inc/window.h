#ifndef RYUJINIII_GUI_WINDOW_H
#define RYUJINIII_GUI_WINDOW_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/listitem.h>
#include <gtkmm/listview.h>
#include <gtkmm/multiselection.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separator.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/window.h>
#include <thread>

#include "GTKMMCustomEntry.h"
#include "checkbutton_media.h"
#include "commands/hardware_monitor_chain.h"
#include "ryujin/ryujin_device.h"
#include "stores/memory_slots_store.h"

class Window : public Gtk::Window {
public:
    Window(RyujinDevice &device);
    ~Window() = default;

private:
    //sections config
    void ConfigureSection1();
    void ConfigureSection2();
    void ConfigureSection3();
    void ConfigureSection4();
    void ConfigureSection5();
    void ConfigureSection6();
    void ConfigureSeparatorSection();

    void EnableAll();
    void DisableButtonList(std::vector<Gtk::Button *> enabled_buttons);
    void OpenFileDialog();
    void OnFileSelect(const Glib::RefPtr<Gio::AsyncResult> &result, const Glib::RefPtr<Gtk::FileDialog> &dialog);
    void InitializeImplementations();
    void SetListLabels(const Glib::RefPtr<Gtk::StringList> &list, const Glib::RefPtr<Gtk::ListItem> &list_item);
    void SetListValues(const Glib::RefPtr<Gtk::StringList> &list, const Glib::RefPtr<Gtk::ListItem> &list_item);

    //Actions
    void ExecuteCommand(ExecuteBase *command);
    void TurnOnAction();
    void TurnOffAction();
    void DefaultGIFAction();
    void ClockButtonAction();
    void MediaButtonSelectionAction(RyujinConstants::Slots slot_type);
    void MediaButtonDisableAll(RyujinConstants::Slots slot_type);
    void MediaButtonEnableAll(RyujinConstants::Slots slot_type);
    void DeleteAction();
    void SlideshowAction();
    void SelectAction();
    void UnselectAction();
    void PumpAction();
    void FanAction();
    void BothConfigAction();
    void ImplementationsSelectionAction(guint position, guint no_items);
    void ModesSelectionAction(guint position, guint no_items);
    void StylesSelectionAction(guint position, guint no_items);
    void StopMonitorAction();
    void DefaultMonitorAction();
    void StartMonitorAction();

    int GetValueFromDigitInput(const Gtk::Entry &entry);
    void InitializeSlots(std::array<std::string, 10> slots, RyujinConstants::Slots slot_type, std::array<CheckbuttonMedia *, 10> &media_button_slots);

    static constexpr int kDefaultMargins = 15;
    static constexpr int kDefaultSpacing = 10;

    RyujinDevice &device_;
    RyujinConstants::Slots slot_type_;
    int index_;
    std::vector<Gtk::Button *> buttons_;
    Gtk::Grid grid;
    std::thread monitor_thread_;
    HardwareMonitorChain hardware_monitor_chain_;


    //separator section
    Gtk::Separator separator_;
    Gtk::Box separator_section_;

    //section 1
    Gtk::Box section1_;
    Gtk::Label version_;
    Gtk::Button turnon_button_;
    Gtk::Button turnoff_button_;
    Gtk::Button default_gif_button_;
    Gtk::Button clock_button_;

    //section 2
    Gtk::Box section2_;
    Gtk::Label gif_slots_label_;
    Gtk::Label jpeg_slots_label_;
    Gtk::Box gif_slots_box_;
    Gtk::Box jpeg_slots_box_;
    Gtk::ScrolledWindow scroll_window_gif_slots_;
    Gtk::ScrolledWindow scroll_window_jpeg_slots_;
    std::array<CheckbuttonMedia *, 10> gif_slots_;
    std::array<CheckbuttonMedia *, 10> jpeg_slots_;

    //section 3
    Gtk::Box section3_;
    Gtk::Button select_button_;
    Gtk::Button upload_button_;
    Gtk::Button slideshow_button_;
    Gtk::Button unselect_button_;
    Gtk::Button delete_button_;
    Gtk::Label jpeg_slideshow_seconds_label_;
    GTKMMCustomEntry jpeg_slideshow_seconds_entry_;

    //section 4
    Gtk::Box section4_;
    Gtk::Button fan_config_button_;
    Gtk::Label fan_config_value_;
    GTKMMCustomEntry fan_config_input_;
    Gtk::Button pump_config_button_;
    Gtk::Label pump_config_value_;
    GTKMMCustomEntry pump_config_input_;
    Gtk::Button config_both_button_;

    //section 5
    Gtk::Box section5_;
    const int kDefaultBoxListsSpacing = 5;
    Gtk::Box horizontalbox_lists_;
    Gtk::Box verticalbox_implementations_;
    Gtk::Box verticalbox_modes_;
    Gtk::Box verticalbox_styles_;
    Gtk::Label header_implementations_;
    Gtk::Label header_modes_;
    Gtk::Label header_lists_;
    Glib::RefPtr<Gtk::SignalListItemFactory> factory_implementations_;
    Glib::RefPtr<Gtk::SignalListItemFactory> factory_modes_;
    Glib::RefPtr<Gtk::SignalListItemFactory> factory_styles_;
    Glib::RefPtr<Gtk::StringList> implementations_lists_;
    Gtk::ListView implementations_lists_view_;
    Glib::RefPtr<Gtk::StringList> modes_lists_;
    Gtk::ListView modes_lists_view_;
    Glib::RefPtr<Gtk::StringList> styles_lists_;
    Gtk::ListView styles_lists_view_;
    Glib::RefPtr<Gtk::MultiSelection> implementations_model_;
    Glib::RefPtr<Gtk::SingleSelection> modes_model_;
    Glib::RefPtr<Gtk::SingleSelection> styles_model_;

    //section 6
    Gtk::Box section6_;
    Gtk::Button default_monitor_button_;
    Gtk::Button start_monitor_button_;
    Gtk::Button stop_monitor_button_;

};

#endif // RYUJINIII_GUI_WINDOW_H
