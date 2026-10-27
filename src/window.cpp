#include "window.h"

#include <giomm/liststore.h>
#include <gtkmm/error.h>
#include <iostream>

#include "commands/clock_chain.h"
#include "commands/default_gif_command.h"
#include "commands/delete_chain.h"
#include "commands/hardware_monitor_chain.h"
#include "commands/hardware_monitor_chain_config.h"
#include "commands/select_gif_command.h"
#include "commands/select_jpeg_chain.h"
#include "commands/slide_show_chain.h"
#include "commands/slide_show_jpeg_chain.h"
#include "commands/speed_config_command.h"
#include "commands/turn_off_command.h"
#include "commands/turn_on_command.h"
#include "commands/upload_chain.h"
#include "commands/upload_chain_jpeg.h"
#include "stores/memory_slots_store.h"
#include "tools/magick_tool.h"
#include "tools/magick_tool_jpeg.h"
#include "utils/file_handle.h"
#include "version.h"

Window::Window(RyujinDevice &device) : device_(device), hardware_monitor_chain_(this->device_.GetWrapper()) {
    grid = Gtk::Grid();
    this->ConfigureSection1();
    this->ConfigureSection2();
    this->ConfigureSection3();
    this->ConfigureSeparatorSection();
    this->ConfigureSection4();
    this->ConfigureSection5();
    this->ConfigureSection6();

    grid.set_column_homogeneous(true);
    grid.attach(section1_, 0, 0, 1, 4);
    grid.attach(section2_, 1, 0, 4, 4);
    grid.attach(section3_, 5, 0, 1, 4);
    grid.attach(separator_section_, 0, 4, 6, 1);
    grid.attach(section4_, 0, 5, 1, 4);
    grid.attach(section5_, 1, 5, 3, 4);
    grid.attach(section6_, 5, 5, 1, 4);
    set_child(grid);
}

void Window::ConfigureSection1() {
    section1_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    version_ = Gtk::Label(RYUJINIII_GUI_VERSION);
    section1_.set_margin(kDefaultMargins);
    turnon_button_ = Gtk::Button("Turn on");
    turnoff_button_ = Gtk::Button("Turn off");
    default_gif_button_ = Gtk::Button("Default gif");
    clock_button_ = Gtk::Button("Clock mode");

    this->buttons_.push_back(&turnoff_button_);
    this->buttons_.push_back(&turnon_button_);
    this->buttons_.push_back(&default_gif_button_);
    this->buttons_.push_back(&clock_button_);

    section1_.append(version_);
    section1_.append(turnon_button_);
    section1_.append(turnoff_button_);
    section1_.append(default_gif_button_);
    section1_.append(clock_button_);
    section1_.set_valign(Gtk::Align::START);
    section1_.set_halign(Gtk::Align::FILL);

    turnon_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::TurnOnAction), false);
    turnoff_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::TurnOffAction), false);
    default_gif_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::DefaultGIFAction), false);
    clock_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::ClockButtonAction), false);
}
void Window::ConfigureSection2() {
    section2_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    gif_slots_label_ = Gtk::Label("GIF Slots");
    jpeg_slots_label_ = Gtk::Label("JPEG Slots");
    scroll_window_gif_slots_ = Gtk::ScrolledWindow();
    scroll_window_jpeg_slots_ = Gtk::ScrolledWindow();
    gif_slots_box_ = Gtk::Box(Gtk::Orientation::HORIZONTAL, kDefaultSpacing);
    jpeg_slots_box_ = Gtk::Box(Gtk::Orientation::HORIZONTAL, kDefaultSpacing);


    scroll_window_gif_slots_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER);
    scroll_window_gif_slots_.set_child(gif_slots_box_);
    scroll_window_gif_slots_.set_expand();

    scroll_window_jpeg_slots_.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::NEVER);
    scroll_window_jpeg_slots_.set_child(jpeg_slots_box_);
    scroll_window_jpeg_slots_.set_expand();

    MemorySlotsStore memory_slots_store(RyujinConstants::kRyujinPersistentDirectory);
    InitializeSlots(memory_slots_store.GetSlotsByType(), RyujinConstants::Slots::GIF, this->gif_slots_);
    InitializeSlots(memory_slots_store.GetSlotsByType(RyujinConstants::Slots::JPEG), RyujinConstants::Slots::JPEG,
                    this->jpeg_slots_);

    section2_.set_margin(kDefaultMargins);
    section2_.set_valign(Gtk::Align::START);
    section2_.set_halign(Gtk::Align::FILL);
    section2_.set_hexpand(true);
    section2_.set_vexpand(false);
    section2_.append(gif_slots_label_);
    section2_.append(scroll_window_gif_slots_);
    section2_.append(jpeg_slots_label_);
    section2_.append(scroll_window_jpeg_slots_);
}
void Window::ConfigureSection3() {
    section3_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    jpeg_slideshow_seconds_label_ = Gtk::Label("Slideshow Seconds");
    jpeg_slideshow_seconds_entry_ = GTKMMCustomEntry(1, 255);
    upload_button_ = Gtk::Button("Upload");
    slideshow_button_ = Gtk::Button("Slideshow");
    slideshow_button_.set_sensitive(false);
    slideshow_button_.set_hexpand(true);
    unselect_button_ = Gtk::Button("Unselect All");
    unselect_button_.set_sensitive(false);
    select_button_ = Gtk::Button("Select");
    delete_button_ = Gtk::Button("Delete");
    delete_button_.set_hexpand(true);

    this->buttons_.push_back(&select_button_);
    this->buttons_.push_back(&upload_button_);
    this->buttons_.push_back(&slideshow_button_);
    this->buttons_.push_back(&unselect_button_);
    this->buttons_.push_back(&delete_button_);

    section3_.set_margin(kDefaultMargins);

    section3_.append(upload_button_);
    section3_.append(delete_button_);
    section3_.append(select_button_);
    section3_.append(slideshow_button_);
    section3_.append(jpeg_slideshow_seconds_label_);
    section3_.append(jpeg_slideshow_seconds_entry_);
    section3_.append(unselect_button_);
    section3_.set_valign(Gtk::Align::START);
    section3_.set_halign(Gtk::Align::FILL);

    select_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::SelectAction), false);
    upload_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::OpenFileDialog), false);
    delete_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::DeleteAction), false);
    slideshow_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::SlideshowAction), false);
    unselect_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::UnselectAction), false);
}
void Window::ConfigureSection4() {
    section4_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    pump_config_button_ = Gtk::Button("Pump Config");
    pump_config_value_ = Gtk::Label("Pump Value");
    pump_config_input_ = GTKMMCustomEntry(1, 100);
    fan_config_button_ = Gtk::Button("Fan Config");
    fan_config_value_ = Gtk::Label("Fan Value");
    fan_config_input_ = GTKMMCustomEntry(1, 100);
    config_both_button_ = Gtk::Button("Config Both");

    section4_.set_margin(kDefaultMargins);

    section4_.append(pump_config_button_);
    section4_.append(pump_config_value_);
    section4_.append(pump_config_input_);
    section4_.append(fan_config_button_);
    section4_.append(fan_config_value_);
    section4_.append(fan_config_input_);
    section4_.append(config_both_button_);

    pump_config_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::PumpAction));
    fan_config_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::FanAction));
    config_both_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::BothConfigAction));
}
void Window::ConfigureSection5() {
    section5_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    this->InitializeImplementations();
    modes_lists_ = Gtk::StringList::create({"GALACTIC", "PUNK"});
    styles_lists_ = Gtk::StringList::create({"STYLE01", "STYLE02", "STYLE03", "STYLE04"});
    horizontalbox_lists_ = Gtk::Box(Gtk::Orientation::HORIZONTAL, kDefaultBoxListsSpacing);
    verticalbox_implementations_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultBoxListsSpacing);
    verticalbox_modes_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultBoxListsSpacing);
    verticalbox_styles_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultBoxListsSpacing);
    header_implementations_ = Gtk::Label("<b>Ryujin Implementations</b>");
    header_modes_ = Gtk::Label("<b>Ryujin Modes</b>");
    header_lists_ = Gtk::Label("<b>Ryujin Styles</b>");
    implementations_model_ = Gtk::MultiSelection::create(implementations_lists_);
    modes_model_ = Gtk::SingleSelection::create(modes_lists_);
    styles_model_ = Gtk::SingleSelection::create(styles_lists_);

    header_implementations_.set_use_markup(true);
    header_modes_.set_use_markup(true);
    header_lists_.set_use_markup(true);
    header_implementations_.set_halign(Gtk::Align::START);
    header_modes_.set_halign(Gtk::Align::START);
    header_lists_.set_halign(Gtk::Align::START);

    section5_.set_margin(kDefaultMargins);
    section5_.set_valign(Gtk::Align::START);
    section5_.set_halign(Gtk::Align::FILL);

    implementations_lists_view_.set_model(implementations_model_);
    implementations_lists_view_.add_css_class("data-table");
    implementations_lists_view_.set_size_request(200, 100);

    modes_lists_view_.set_model(modes_model_);
    modes_lists_view_.add_css_class("data-table");
    modes_lists_view_.set_size_request(200, 100);

    styles_lists_view_.set_model(styles_model_);
    styles_lists_view_.add_css_class("data-table");
    styles_lists_view_.set_size_request(200, 100);

    factory_implementations_ = Gtk::SignalListItemFactory::create();
    factory_modes_ = Gtk::SignalListItemFactory::create();
    factory_styles_ = Gtk::SignalListItemFactory::create();
    factory_implementations_->signal_setup().connect(std::bind(sigc::mem_fun(*this, &Window::SetListLabels), implementations_lists_, std::placeholders::_1), false);
    factory_implementations_->signal_bind().connect(std::bind(sigc::mem_fun(*this, &Window::SetListValues), implementations_lists_, std::placeholders::_1), false);

    factory_modes_->signal_setup().connect(std::bind(sigc::mem_fun(*this, &Window::SetListLabels), modes_lists_, std::placeholders::_1), false);
    factory_modes_->signal_bind().connect(std::bind(sigc::mem_fun(*this, &Window::SetListValues), modes_lists_, std::placeholders::_1), false);

    factory_styles_->signal_setup().connect(std::bind(sigc::mem_fun(*this, &Window::SetListLabels), styles_lists_, std::placeholders::_1), false);
    factory_styles_->signal_bind().connect(std::bind(sigc::mem_fun(*this, &Window::SetListValues), styles_lists_, std::placeholders::_1), false);

    implementations_lists_view_.set_factory(factory_implementations_);
    modes_lists_view_.set_factory(factory_modes_);
    styles_lists_view_.set_factory(factory_styles_);


    verticalbox_implementations_.append(header_implementations_);
    verticalbox_implementations_.append(implementations_lists_view_);

    verticalbox_modes_.append(header_modes_);
    verticalbox_modes_.append(modes_lists_view_);

    verticalbox_styles_.append(header_lists_);
    verticalbox_styles_.append(styles_lists_view_);

    horizontalbox_lists_.append(verticalbox_implementations_);
    horizontalbox_lists_.append(verticalbox_modes_);
    horizontalbox_lists_.append(verticalbox_styles_);


    section5_.append(horizontalbox_lists_);

    implementations_model_->signal_selection_changed().connect(sigc::mem_fun(*this, &Window::ImplementationsSelectionAction));
    modes_model_->signal_selection_changed().connect(sigc::mem_fun(*this, &Window::ModesSelectionAction));
    styles_model_->signal_selection_changed().connect(sigc::mem_fun(*this, &Window::StylesSelectionAction));
}
void Window::ConfigureSection6() {
    section6_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    default_monitor_button_ = Gtk::Button("Default monitor");
    start_monitor_button_ = Gtk::Button("Start monitor");
    stop_monitor_button_ = Gtk::Button("Stop monitor");

    section6_.set_margin(kDefaultMargins);
    section6_.set_valign(Gtk::Align::START);
    section6_.set_halign(Gtk::Align::FILL);
    start_monitor_button_.set_sensitive(false);

    this->buttons_.push_back(&default_monitor_button_);
    this->buttons_.push_back(&start_monitor_button_);
    this->buttons_.push_back(&stop_monitor_button_);

    section6_.append(default_monitor_button_);
    section6_.append(start_monitor_button_);
    section6_.append(stop_monitor_button_);

    stop_monitor_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::StopMonitorAction),false);
    default_monitor_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::DefaultMonitorAction),false);
    start_monitor_button_.signal_clicked().connect(sigc::mem_fun(*this, &Window::StartMonitorAction),false);
}
void Window::ConfigureSeparatorSection() {
    separator_section_ = Gtk::Box(Gtk::Orientation::VERTICAL, kDefaultSpacing);
    separator_section_.set_margin(kDefaultMargins);
    separator_ = Gtk::Separator();
    separator_.set_valign(Gtk::Align::START);
    separator_section_.append(separator_);
}
void Window::EnableAll() {
    for (auto &button: this->buttons_) {
        button->set_sensitive(true);
    }
}
void Window::DisableButtonList(std::vector<Gtk::Button *> enabled_buttons) {
    for (auto &button: this->buttons_) {
        if (std::find(enabled_buttons.begin(), enabled_buttons.end(), button) == enabled_buttons.end()) {
            button->set_sensitive(false);
        } else {
            button->set_sensitive(true);
        }
    }
}
void Window::OpenFileDialog() {
    auto dialog = Gtk::FileDialog::create();
    auto filters = Gio::ListStore<Gtk::FileFilter>::create();
    auto filter_gif = Gtk::FileFilter::create();
    if (this->slot_type_ == RyujinConstants::Slots::GIF) {
        filter_gif->set_name("GIF files only");
        filter_gif->add_mime_type("image/gif");
        filters->append(filter_gif);
    } else if (this->slot_type_ == RyujinConstants::Slots::JPEG) {
        filter_gif->set_name("JPG files only");
        filter_gif->add_mime_type("image/jpg");
        filters->append(filter_gif);
        filter_gif->set_name("JPEG files only");
        filter_gif->add_mime_type("image/jpeg");
        filters->append(filter_gif);
    }
    dialog->set_modal(true);
    dialog->set_filters(filters);
    dialog->open(sigc::bind(sigc::mem_fun(*this, &Window::OnFileSelect), dialog));
}
void Window::OnFileSelect(const Glib::RefPtr<Gio::AsyncResult> &result, const Glib::RefPtr<Gtk::FileDialog> &dialog) {
    try {
        auto file = dialog->open_finish(result);
        auto filename = file->get_path();
        std::shared_ptr<ExecuteBase> command;
        std::shared_ptr<MemorySlotsStoreBase> store =
                std::make_shared<MemorySlotsStore>(RyujinConstants::kRyujinPersistentDirectory);
        if (this->slot_type_ == RyujinConstants::Slots::GIF) {
            command = std::make_shared<UploadChain>(std::make_shared<MagickTool>(), std::make_shared<FileHandle>(),
                                                    this->device_.GetWrapper(), filename, index_, store);
        } else if (this->slot_type_ == RyujinConstants::Slots::JPEG) {
            command = std::make_shared<UploadChainJpeg>(std::make_shared<MagickToolJpeg>(),
                                                        std::make_shared<FileHandle>(), this->device_.GetWrapper(),
                                                        filename, index_, store);
        }
        if (command) {
            command->Execute();
        }
        MemorySlotsStore memory_slots_store(RyujinConstants::kRyujinPersistentDirectory);
        if (this->slot_type_ == RyujinConstants::Slots::GIF) {
            gif_slots_box_.remove(*gif_slots_[index_]);
            auto slots = memory_slots_store.GetSlotsByType();
            auto new_gif_slot = new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::GIF, slots[index_]);
            new_gif_slot->set_size_request(120, 100);
            if (index_ > 0) {
                gif_slots_box_.insert_child_after(*new_gif_slot, *gif_slots_[index_ - 1]);
            } else {
                gif_slots_box_.insert_child_at_start(*new_gif_slot);
            }
            delete gif_slots_[index_];
            gif_slots_[index_] = new_gif_slot;
        } else if (this->slot_type_ == RyujinConstants::Slots::JPEG) {
            jpeg_slots_box_.remove(*jpeg_slots_[index_]);
            auto slots = memory_slots_store.GetSlotsByType(RyujinConstants::Slots::JPEG);
            auto new_jpeg_slot = new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::IMAGE, slots[index_]);
            new_jpeg_slot->set_size_request(120, 100);
            if (index_ > 0) {
                jpeg_slots_box_.insert_child_after(*new_jpeg_slot, *jpeg_slots_[index_ - 1]);
            } else {
                jpeg_slots_box_.insert_child_at_start(*new_jpeg_slot);
            }
            delete jpeg_slots_[index_];
            jpeg_slots_[index_] = new_jpeg_slot;
        }
    } catch (const Gtk::DialogError &err) {
    } catch (const Glib::Error &err) {
    }
}
void Window::InitializeImplementations() {
    std::string implements = LedLineFactory::PrintCurrentImplements();
    auto first_line = implements.find_first_of("\n");
    std::string segment;
    auto substring = std::stringstream(implements.substr(first_line + 1));
    implementations_lists_ = Gtk::StringList::create();
    while (std::getline(substring, segment)) {
        implementations_lists_->append(segment);
    }
}
void Window::SetListLabels(const Glib::RefPtr<Gtk::StringList> &list, const Glib::RefPtr<Gtk::ListItem> &list_item) {
    list_item->set_child(*Gtk::make_managed<Gtk::Label>("", Gtk::Align::START));
}
void Window::SetListValues(const Glib::RefPtr<Gtk::StringList> &list, const Glib::RefPtr<Gtk::ListItem> &list_item) {
    auto pos = list_item->get_position();
    if (pos == GTK_INVALID_LIST_POSITION) return;
    auto label = dynamic_cast<Gtk::Label *>(list_item->get_child());
    if (!label) return;
    label->set_text(list->get_string(pos));
}
void Window::ExecuteCommand(ExecuteBase *command) {
    if (!command->Execute()) {
        std::cout << "Command execution fail: " << command->GetClassName() << std::endl;
    }
}
void Window::TurnOnAction() {
    TurnOnCommand command(this->device_.GetWrapper());
    this->ExecuteCommand(&command);
}
void Window::TurnOffAction() {
    TurnOffCommand command(this->device_.GetWrapper());
    this->ExecuteCommand(&command);
}
void Window::DefaultGIFAction() {
    DefaultGifCommand command(this->device_.GetWrapper());
    this->ExecuteCommand(&command);
}
void Window::ClockButtonAction() {
    ClockChain command(this->device_.GetWrapper());
    this->ExecuteCommand(&command);
}
void Window::MediaButtonSelectionAction(RyujinConstants::Slots slot_type) {
    this->slot_type_ = slot_type;
    auto media_button_slots = slot_type == RyujinConstants::Slots::GIF ? this->gif_slots_ : this->jpeg_slots_;
    int selected = 0;
    for (int i = 0; i < media_button_slots.size(); i++) {
        if (media_button_slots[i]->IsSelected()) {
            this->index_ = i;
            selected++;
        }
    }
    if (selected) {
        this->MediaButtonDisableAll(slot_type);
    } else {
        this->MediaButtonEnableAll(slot_type);
    }
    if (selected > 1) {
        this->upload_button_.set_sensitive(false);
        this->select_button_.set_sensitive(false);
        this->slideshow_button_.set_sensitive(true);
        this->unselect_button_.set_sensitive(true);
    } else {
        this->upload_button_.set_sensitive(true);
        this->select_button_.set_sensitive(true);
        this->slideshow_button_.set_sensitive(false);
        this->unselect_button_.set_sensitive(false);
    }
}
void Window::MediaButtonDisableAll(RyujinConstants::Slots slot_type) {
    auto slots = slot_type == RyujinConstants::Slots::GIF ? &this->jpeg_slots_ : &this->gif_slots_;
    for (CheckbuttonMedia *reference: *slots) {
        reference->DisableSelection();
    }
}
void Window::MediaButtonEnableAll(RyujinConstants::Slots slot_type) {
    auto slots = slot_type == RyujinConstants::Slots::GIF ? &this->jpeg_slots_ : &this->gif_slots_;
    for (CheckbuttonMedia *reference: *slots) {
        reference->EnableSelection();
    }
}
void Window::DeleteAction() {
    auto slots = slot_type_ == RyujinConstants::Slots::GIF ? &gif_slots_ : &jpeg_slots_;
    auto slots_box = slot_type_ == RyujinConstants::Slots::GIF ? &gif_slots_box_ : &jpeg_slots_box_;
    std::shared_ptr<MemorySlotsStoreBase> store =
            std::make_shared<MemorySlotsStore>(RyujinConstants::kRyujinPersistentDirectory);
    for (int i = 0; i < 10; i++) {
        if ((*slots)[i]->IsSelected()) {
            DeleteChain delete_chain(this->device_.GetWrapper(), i, store, slot_type_);
            if (delete_chain.Execute()) {
                auto new_media_button =
                        new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::IMAGE, "empty.svg");
                new_media_button->set_size_request(120, 100);
                slots_box->remove(*(*slots)[i]);
                if (i > 0) {
                    slots_box->insert_child_after(*new_media_button, *(*slots)[i - 1]);
                } else {
                    slots_box->insert_child_at_start(*new_media_button);
                }
                delete (*slots)[i];

                (*slots)[i] = new_media_button;
            }
        }
    }
}
void Window::SlideshowAction() {
    std::vector<int> indexes;
    auto slots = slot_type_ == RyujinConstants::Slots::GIF ? &gif_slots_ : &jpeg_slots_;
    int seconds = 5;
    if (slot_type_ == RyujinConstants::Slots::JPEG) {
        try {
            seconds = std::stoi(this->jpeg_slideshow_seconds_entry_.get_text());
        } catch (std::exception &e) {
            seconds = 5;
        }
    }
    for (int i = 0; i < 10; i++) {
        if ((*slots)[i]->IsSelected()) {
            indexes.push_back(i);
        }
    }
    std::unique_ptr<ExecuteBase> command;
    if (slot_type_ == RyujinConstants::Slots::GIF) {
        command = std::make_unique<SlideShowChain>(this->device_.GetWrapper(), indexes);
    } else {
        command = std::make_unique<SlideShowJpegChain>(this->device_.GetWrapper(), indexes, seconds);
    }
    this->ExecuteCommand(command.get());
}
void Window::SelectAction() {
    std::shared_ptr<ExecuteBase> command;
    if (this->slot_type_ == RyujinConstants::Slots::GIF) {
        command = std::make_shared<SelectGifCommand>(this->device_.GetWrapper(), this->index_);
    } else if (this->slot_type_ == RyujinConstants::Slots::JPEG) {
        command = std::make_shared<SelectJpegChain>(this->device_.GetWrapper(), this->index_);
    }
    this->ExecuteCommand(command.get());
}
void Window::UnselectAction() {
    for (auto &jpeg_slot: jpeg_slots_) {
        jpeg_slot->DisableSelection();
        jpeg_slot->EnableSelection();
    }
    for (auto &gif_slot: gif_slots_) {
        gif_slot->DisableSelection();
        gif_slot->EnableSelection();
    }
}
void Window::PumpAction() {
    int value = GetValueFromDigitInput(pump_config_input_);
    SpeedConfigCommand speed_config_command(this->device_.GetWrapper(),
                                            RyujinConstants::DeviceSelector::PUMP_DEVICE, value);
    ExecuteCommand(&speed_config_command);
}
void Window::FanAction() {
    int value = GetValueFromDigitInput(fan_config_input_);
    SpeedConfigCommand speed_config_command(this->device_.GetWrapper(), RyujinConstants::DeviceSelector::FAN_DEVICE,
                                            value);
    ExecuteCommand(&speed_config_command);
}
void Window::BothConfigAction() {
    auto fan_value = GetValueFromDigitInput(fan_config_input_);
    auto pump_value = GetValueFromDigitInput(pump_config_input_);
    SpeedConfigCommand speed_config_command(this->device_.GetWrapper(), pump_value, fan_value);
    ExecuteCommand(&speed_config_command);
}
void Window::ImplementationsSelectionAction(guint position, guint no_items) {
    if (implementations_model_->get_selection()->get_size() > 3) {
        implementations_model_->unselect_item(position);
    }
    if (implementations_model_->get_selection()->get_size() && modes_model_->get_selection()->get_size() &&
        styles_model_->get_selection()->get_size()) {
        start_monitor_button_.set_sensitive(true);
        } else {
            start_monitor_button_.set_sensitive(false);
        }
}
void Window::ModesSelectionAction(guint position, guint no_items) {
    if (implementations_model_->get_selection()->get_size() && modes_model_->get_selection()->get_size() &&
            styles_model_->get_selection()->get_size()) {
        start_monitor_button_.set_sensitive(true);
            } else {
                start_monitor_button_.set_sensitive(false);
            }
}
void Window::StylesSelectionAction(guint position, guint no_items) {
    if (implementations_model_->get_selection()->get_size() && modes_model_->get_selection()->get_size() &&
           styles_model_->get_selection()->get_size()) {
        start_monitor_button_.set_sensitive(true);
           } else {
               start_monitor_button_.set_sensitive(false);
           }
}
void Window::StopMonitorAction() {
    if (this->monitor_thread_.joinable()) {
        this->hardware_monitor_chain_.StopLoop(11);
        this->monitor_thread_.join();
        this->EnableAll();
        for (auto &media_button: this->gif_slots_) {
            media_button->EnableSelection();
        }
        for (auto &media_button: this->jpeg_slots_) {
            media_button->EnableSelection();
        }
    }
}
void Window::DefaultMonitorAction() {
    if (this->monitor_thread_.joinable()) {
        this->hardware_monitor_chain_.StopLoop(1);
        this->monitor_thread_.join();
    }
    this->monitor_thread_ = std::thread(&HardwareMonitorChain::Execute, hardware_monitor_chain_);
    this->DisableButtonList(std::vector<Gtk::Button *>{&stop_monitor_button_});
    for (auto &media_button: this->gif_slots_) {
        media_button->DisableSelection();
    }
    for (auto &media_button: this->jpeg_slots_) {
        media_button->DisableSelection();
    }
}
void Window::StartMonitorAction() {
    HardwareMonitorChainConfig hardware_monitor_chain_config(this->device_.GetWrapper());
    if (this->monitor_thread_.joinable()) {
        this->hardware_monitor_chain_.StopLoop(1);
        this->monitor_thread_.join();
    }
    for (auto led_line: *implementations_model_->get_selection()) {
        hardware_monitor_chain_config.AddLine(led_line);
    }
    hardware_monitor_chain_config.SetMode(modes_model_->get_selection().get()->get_nth(0));
    hardware_monitor_chain_config.SetStyle(styles_model_->get_selection().get()->get_nth(0));
    this->hardware_monitor_chain_ = *hardware_monitor_chain_config.GetChain();
    this->monitor_thread_ = std::thread(&HardwareMonitorChain::Execute, hardware_monitor_chain_);
    this->DisableButtonList(std::vector<Gtk::Button *>{&stop_monitor_button_});
    for (auto &media_button: this->gif_slots_) {
        media_button->DisableSelection();
    }
    for (auto &media_button: this->jpeg_slots_) {
        media_button->DisableSelection();
    }
}
int Window::GetValueFromDigitInput(const Gtk::Entry &entry) {
    int value = 0;
    try {
        value = std::stoi(entry.get_text());
    }catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        value = 0;
    }
    return value;
}
void Window::InitializeSlots(std::array<std::string, 10> slots, RyujinConstants::Slots slot_type,
                             std::array<CheckbuttonMedia *, 10> &media_button_slots) {
    auto slots_box = slot_type == RyujinConstants::Slots::GIF ? &gif_slots_box_ : &jpeg_slots_box_;
    for (int i = 0; i < slots.size(); i++) {
        if (slots[i].empty()) {
            media_button_slots[i] = new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::IMAGE, "empty.svg");
        } else if (slot_type == RyujinConstants::Slots::GIF) {
            media_button_slots[i] = new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::GIF, slots[i]);
        } else if (slot_type == RyujinConstants::Slots::JPEG) {
            media_button_slots[i] = new CheckbuttonMedia(CheckbuttonMedia::CheckbuttonMediaType::IMAGE, slots[i]);
        }
        media_button_slots[i]->set_size_request(120, 100);
        media_button_slots[i]->signal_changed().connect(
                sigc::bind(sigc::mem_fun(*this, &Window::MediaButtonSelectionAction), slot_type));
        slots_box->append(*media_button_slots[i]);
    }
}

