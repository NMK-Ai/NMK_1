#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <cmath>
#include <string>

#include <QDebug>
#include <QProcess> // opkr
#include <QDateTime> // opkr
#include <QTimer> // opkr
#include <QFileInfo> // opkr

#ifndef QCOM
#include "selfdrive/ui/qt/offroad/networking.h"
#endif

#ifdef ENABLE_MAPS
#include "selfdrive/ui/qt/maps/map_settings.h"
#endif

#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/widgets/ssh_keys.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/qt_window.h"

#include "selfdrive/ui/qt/widgets/opkr.h"
#include "selfdrive/ui/qt/widgets/steerWidget.h"


TogglesPanel::TogglesPanel(SettingsWindow *parent) : ListWidget(parent) {
  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggles{
    {
      "OpenpilotEnabledToggle",
      tr("تمكين القائد الآلي"),
      tr("استخدم نظام القائد الآلي للتحكم التكيفي في ثبات السرعة والحفاظ على مساعدة السائق. انتباهك مطلوب في جميع الأوقات لاستخدام هذه الميزة. يسري تغيير هذا الإعداد عند إيقاف تشغيل السيارة."),
      "../assets/offroad/icon_openpilot.png",
    },
    {
      "IsLdwEnabled",
      tr("تفعيل تحذيرات مغادرة حارة السير"),
      tr("تلقي تنبيهات للتوجيه مرة أخرى إلى الحارة عندما تنجرف سيارتك فوق خط المسار المكتشف دون تنشيط إشارة الانعطاف أثناء القيادة لمسافة تزيد عن 31 ميلاً في الساعة (50 كم / ساعة)."),
      "../assets/offroad/icon_warning.png",
    },
    {
      "IsRHD",
      tr("تمكين القيادة باليد اليمنى"),
      tr("اسمح للقائد الآلي بالانصياع لاتفاقيات المرور اليسرى وإجراء مراقبة للسائق على مقعد السائق الأيمن."),
      "../assets/offroad/icon_openpilot_mirrored.png",
    },
    {
      "IsMetric",
      tr("استخدم النظام المتري"),
      tr("عرض السرعة بالكيلو متر في الساعة بدلا من ميلا في الساعة."),
      "../assets/offroad/icon_metric.png",
    },
    {
      "RecordFront",
      tr("تسجيل وتحميل كاميرا السائق"),
      tr("قم بتحميل البيانات من الكاميرا المواجهة للسائق وساعد في تحسين خوارزمية مراقبة السائق."),
      "../assets/offroad/icon_monitoring.png",
    },
    {
      "EndToEndToggle",
      "\U0001f96c"+tr("قم بتمكين وضع محدد المسار")+"\U0001f96c",
      tr("قم بتنشيط وضع اختيار الحارة. يمكن تحديد وضع المسار و لين أقل و تلقائي وتشغيله على الشاشة."),
      "../assets/offroad/icon_road.png",
    },
#ifdef ENABLE_MAPS
    {
      "NavSettingTime24h",
      tr("إظهار الوقت المقدر للوصول بتنسيق 24 ساعة"),
      tr("استخدم تنسيق 24 ساعة بدلاً من صباحًا / مساءً"),
      "../assets/offroad/icon_metric.png",
    },
#endif

    {
      "OpkrEnableDriverMonitoring",
      tr("تمكين مراقبة السائق"),
      tr("استخدم وظيفة مراقبة السائق."),
      "../assets/offroad/icon_shell.png",
    },
    {
      "OpkrEnableLogger",
      tr("تفعيل سجل القيادة"),
      tr("سجل سجل القيادة محليًا لتحليل البيانات. يتم تنشيط المسجلين فقط ولا يتم تحميلهم على الخادم."),
      "../assets/offroad/icon_shell.png",
    },
    {
      "OpkrEnableUploader",
      tr("قم بتمكين إرسال السجل إلى الخادم"),
      tr("قم بتنشيط عملية التحميل لإرسال سجلات النظام وبيانات القيادة الأخرى إلى الخادم. قم بتحميله فقط على الطرق الوعرة."),
      "../assets/offroad/icon_shell.png",
    },
  };

  Params params;

  if (params.getBool("DisableRadar_Allow")) {
    toggles.push_back({
      "DisableRadar",
      tr("القائد الآلي التحكم الطولي"),
      tr("سوف يقوم برنامج القائد الآلي بتعطيل رادار السيارة وسيتولى التحكم في الغاز والمكابح. تحذير: يؤدي هذا إلى تعطيل نظام التحذير من الاصدام الأمامي!"),
      "../assets/offroad/icon_speed_limit.png",
    });
  }

  for (auto &[param, title, desc, icon] : toggles) {
    auto toggle = new ParamControl(param, title, desc, icon, this);
    //bool locked = params.getBool((param + "Lock").toStdString());
    //toggle->setEnabled(true);
    //connect(parent, &SettingsWindow::offroadTransition, toggle, &ParamControl::setEnabled);
    addItem(toggle);
  }
}

DevicePanel::DevicePanel(SettingsWindow *parent) : ListWidget(parent) {
  setSpacing(50);
  addItem(new LabelControl(tr("معرف الجهاز"), getDongleId().value_or(tr("410148"))));
  addItem(new LabelControl(tr("الرقم التسلسلي"), params.get("HardwareSerial").c_str()));

  addItem(new OpenpilotView());

  // offroad-only buttons

  auto dcamBtn = new ButtonControl(tr("كاميرا السائق"), tr("عرض"),
                                   tr("قم بمعاينة الكاميرا المواجهة للسائق للتأكد من أن مراقبة السائق تتمتع برؤية جيدة. (vehicle must be off)"));
  connect(dcamBtn, &ButtonControl::clicked, [=]() { emit showDriverView(); });
  addItem(dcamBtn);

  if (!params.getBool("Passive")) {
    auto retrainingBtn = new ButtonControl(tr("مراجعة دليل التدريب"), tr("عرض"), tr("راجع القواعد والميزات والقيود الخاصة بالقائد الآلي"));
    connect(retrainingBtn, &ButtonControl::clicked, [=]() {
      if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد مراجعة دليل التدريب؟"), this)) {
        emit reviewTrainingGuide();
      }
    });
    addItem(retrainingBtn);
  }

  if (Hardware::TICI()) {
    auto regulatoryBtn = new ButtonControl(tr("تنظيمية"), tr("معاينة"), "");
    connect(regulatoryBtn, &ButtonControl::clicked, [=]() {
      const std::string txt = util::read_file("../assets/offroad/fcc.html");
      RichTextDialog::alert(QString::fromStdString(txt), this);
    });
    addItem(regulatoryBtn);
  }

  auto resetCalibBtn = new ButtonControl(tr("إعادة ضبط المعايرة"), tr("إعادة تعيين"), " ");
  connect(resetCalibBtn, &ButtonControl::showDescription, this, &DevicePanel::updateCalibDescription);
  connect(resetCalibBtn, &ButtonControl::clicked, [&]() {
    if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد إعادة ضبط المعايرة؟"), this)) {
      params.remove("CalibrationParams");
      params.remove("LiveParameters");
      params.putBool("OnRoadRefresh", true);
      QTimer::singleShot(3000, [this]() {
        params.putBool("OnRoadRefresh", false);
      });
    }
  });
  addItem(resetCalibBtn);

  auto translateBtn = new ButtonControl(tr("تغيير اللغة"), tr("تغيير"), "");
  connect(translateBtn, &ButtonControl::clicked, [=]() {
    QMap<QString, QString> langs = getSupportedLanguages();
    QString currentLang = QString::fromStdString(Params().get("LanguageSetting"));
    QString selection = MultiOptionDialog::getSelection(tr("اختر لغة"), langs.keys(), langs.key(currentLang), this);
    if (!selection.isEmpty()) {
      // put language setting, exit Qt UI, and trigger fast restart
      Params().put("LanguageSetting", langs[selection].toStdString());
    }
  });
  addItem(translateBtn);

  QObject::connect(parent, &SettingsWindow::offroadTransition, [=](bool offroad) {
    for (auto btn : findChildren<ButtonControl *>()) {
      btn->setEnabled(offroad);
    }
    resetCalibBtn->setEnabled(true);
  });

  // power buttons
  QHBoxLayout *power_layout = new QHBoxLayout();
  power_layout->setSpacing(30);

  QPushButton *refresh_btn = new QPushButton(tr("تحديث"));
  refresh_btn->setObjectName("refresh_btn");
  power_layout->addWidget(refresh_btn);
  QObject::connect(refresh_btn, &QPushButton::clicked, this, &DevicePanel::refresh);

  QPushButton *reboot_btn = new QPushButton(tr("اعادة التشغيل"));
  reboot_btn->setObjectName("reboot_btn");
  power_layout->addWidget(reboot_btn);
  QObject::connect(reboot_btn, &QPushButton::clicked, this, &DevicePanel::reboot);

  QPushButton *poweroff_btn = new QPushButton(tr("إيقاف التشغيل"));
  poweroff_btn->setObjectName("poweroff_btn");
  power_layout->addWidget(poweroff_btn);
  QObject::connect(poweroff_btn, &QPushButton::clicked, this, &DevicePanel::poweroff);

  setStyleSheet(R"(
    QPushButton {
      height: 120px;
      border-radius: 15px;
    }
    #refresh_btn { background-color: #83c744; }
    #refresh_btn:pressed { background-color: #c7deb1; }
    #reboot_btn { background-color: #ed8e3b; }
    #reboot_btn:pressed { background-color: #f0bf97; }
    #poweroff_btn { background-color: #E22C2C; }
    #poweroff_btn:pressed { background-color: #FF2424; }
  )");
  addItem(power_layout);
}

void DevicePanel::updateCalibDescription() {
  QString desc =
      tr("يتطلب برنامج القائد الآلي أن يتم تركيب الجهاز في حدود 4 درجات يسارًا أو يمينًا و "
          "في غضون 5 درجات لأعلى أو 8 درجات لأسفل. يقوم الطيار المفتوح بالمعايرة باستمرار ، ونادراً ما تكون هناك حاجة لإعادة الضبط.");
  std::string calib_bytes = Params().get("CalibrationParams");
  if (!calib_bytes.empty()) {
    try {
      AlignedBuffer aligned_buf;
      capnp::FlatArrayMessageReader cmsg(aligned_buf.align(calib_bytes.data(), calib_bytes.size()));
      auto calib = cmsg.getRoot<cereal::Event>().getLiveCalibration();
      if (calib.getCalStatus() != 0) {
        double pitch = calib.getRpyCalib()[1] * (180 / M_PI);
        double yaw = calib.getRpyCalib()[2] * (180 / M_PI);
        desc += tr(" Your device is pointed %1° %2 and %3° %4.")
                    .arg(QString::number(std::abs(pitch), 'g', 1), pitch > 0 ? tr("أسفل") : tr("أعلى"),
                         QString::number(std::abs(yaw), 'g', 1), yaw > 0 ? tr("يسار") : tr("يمين"));
      }
    } catch (kj::Exception) {
      qInfo() << "invalid CalibrationParams";
    }
  }
  qobject_cast<ButtonControl *>(sender())->setDescription(desc);
}

void DevicePanel::refresh() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد التحديث؟"), this)) {
      // Check engaged again in case it changed while the dialog was open
      if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
        Params().putBool("OnRoadRefresh", true);
        QTimer::singleShot(3000, []() {
          Params().putBool("OnRoadRefresh", false);
        });
      }
    }
  } else {
    ConfirmationDialog::alert(tr("فك الارتباط للتحديث"), this);
  }
}

void DevicePanel::reboot() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد إعادة التشغيل؟"), this)) {
      // Check engaged again in case it changed while the dialog was open
      if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
        Params().putBool("DoReboot", true);
      }
    }
  } else {
    ConfirmationDialog::alert(tr("فك الارتباط لإعادة التشغيل"), this);
  }
}

void DevicePanel::poweroff() {
  if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
    if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد إيقاف التشغيل؟"), this)) {
      // Check engaged again in case it changed while the dialog was open
      if (QUIState::ui_state.status == UIStatus::STATUS_DISENGAGED) {
        Params().putBool("DoShutdown", true);
      }
    }
  } else {
    ConfirmationDialog::alert(tr("فك الارتباط لإيقاف التشغيل"), this);
  }
}

SoftwarePanel::SoftwarePanel(QWidget* parent) : ListWidget(parent) {
  gitRemoteLbl = new LabelControl(tr("بوابة التحكم عن بعد"));
  gitBranchLbl = new LabelControl(tr("فرع جيت"));
  gitCommitLbl = new LabelControl(tr("آخر تعديل"));
  osVersionLbl = new LabelControl(tr("إصدار نظام التشغيل"));
  versionLbl = new LabelControl(tr("فرع"));
  lastUpdateLbl = new LabelControl(tr("التحقق من آخر تحديث"), "", "");
  updateBtn = new ButtonControl(tr("Check for Updates"), "");
  connect(updateBtn, &ButtonControl::clicked, [=]() {
    if (params.getBool("IsOffroad")) {
      fs_watch->addPath(QString::fromStdString(params.getParamPath("LastUpdateTime")));
      fs_watch->addPath(QString::fromStdString(params.getParamPath("UpdateFailedCount")));
    }
    std::system("/data/openpilot/selfdrive/assets/addon/script/gitcommit.sh");
    std::system("date '+%F %T' > /data/params/d/LastUpdateTime");
    QString last_ping = QString::fromStdString(params.get("LastAthenaPingTime"));
    QString desc = "";
    QString commit_local = QString::fromStdString(Params().get("GitCommit").substr(0, 10));
    QString commit_remote = QString::fromStdString(Params().get("GitCommitRemote").substr(0, 10));
    QString empty = "";
    desc += tr("محلي:٪ 1 وحدة تحكم عن بعد:٪ 2٪ 3٪ 4 ").arg(commit_local, commit_remote, empty, empty);
    
    if (!last_ping.length()) {
      desc += tr("اتصال الشبكة مفقود أو غير مستقر. تحقق من الاتصال.");
      ConfirmationDialog::alert(desc, this);
    } else if (commit_local == commit_remote) {
      desc += tr("مطابق محلية وعن بعد. لا حاجة للتحديث.");
      ConfirmationDialog::alert(desc, this);
    } else {
      if (QFileInfo::exists("/data/OPKR_Updates.txt")) {
        QFileInfo fileInfo;
        fileInfo.setFile("/data/OPKR_Updates.txt");
        const std::string txt = util::read_file("/data/OPKR_Updates.txt");
        if (UpdateInfoDialog::confirm(desc + "\n" + QString::fromStdString(txt), this)) {
          std::system("/data/openpilot/selfdrive/assets/addon/script/gitpull.sh");
        }
      } else {
        QString cmd1 = "wget https://raw.githubusercontent.com/openpilotkr/openpilot/"+QString::fromStdString(params.get("GitBranch"))+"/OPKR_Updates.txt -O /data/OPKR_Updates.txt";
        QProcess::execute(cmd1);
        QTimer::singleShot(2000, []() {});
        if (QFileInfo::exists("/data/OPKR_Updates.txt")) {
          QFileInfo fileInfo;
          fileInfo.setFile("/data/OPKR_Updates.txt");
          const std::string txt = util::read_file("/data/OPKR_Updates.txt");
          if (UpdateInfoDialog::confirm(desc + "\n" + QString::fromStdString(txt), this)) {
            std::system("/data/openpilot/selfdrive/assets/addon/script/gitpull.sh");
          }
        }
      }
    }
  });


  auto uninstallBtn = new ButtonControl(tr("الغاء التثبيت %1").arg(getBrand()), tr("الغاء التثبيت"));
  connect(uninstallBtn, &ButtonControl::clicked, [&]() {
    if (ConfirmationDialog::confirm(tr("هل أنت متأكد أنك تريد إلغاء التثبيت؟"), this)) {
      params.putBool("DoUninstall", true);
    }
  });
  connect(parent, SIGNAL(offroadTransition(bool)), uninstallBtn, SLOT(setEnabled(bool)));

  QWidget *widgets[] = {osVersionLbl, versionLbl, gitRemoteLbl, gitBranchLbl, lastUpdateLbl, updateBtn};
  for (QWidget* w : widgets) {
    addItem(w);
  }

  addItem(new GitHash());
 // addItem(new GitPullOnBootToggle());

  addItem(new CPresetWidget());

  addItem(new CGitGroup());




  addItem(new CUtilWidget(this));

  addItem(uninstallBtn);
  fs_watch = new QFileSystemWatcher(this);
  QObject::connect(fs_watch, &QFileSystemWatcher::fileChanged, [=](const QString path) {
    if (path.contains("UpdateFailedCount") && std::atoi(params.get("UpdateFailedCount").c_str()) > 0) {
      lastUpdateLbl->setText(tr("فشل في جلب التحديث"));
      updateBtn->setText(tr("تحقق"));
      updateBtn->setEnabled(true);
    } else if (path.contains("LastUpdateTime")) {
      updateLabels();
    }
  });
}

void SoftwarePanel::showEvent(QShowEvent *event) {
  updateLabels();
}

void SoftwarePanel::updateLabels() {
  QString lastUpdate = "";
  QString tm = QString::fromStdString(params.get("LastUpdateTime").substr(0, 19));
  if (tm != "") {
    lastUpdate = timeAgo(QDateTime::fromString(tm, "yyyy-MM-dd HH:mm:ss"));
  }

  versionLbl->setText("القائد الآلي");
  lastUpdateLbl->setText(lastUpdate);
  updateBtn->setText(tr("فحص"));
  updateBtn->setEnabled(true);
  gitRemoteLbl->setText(QString::fromStdString(params.get("GitRemote").substr(19)));
  gitBranchLbl->setText(QString::fromStdString(params.get("GitBranch")));
  gitCommitLbl->setText(QString::fromStdString(params.get("GitCommit")).left(10));
  osVersionLbl->setText(QString::fromStdString(Hardware::get_os_version()).trimmed());
}

C2NetworkPanel::C2NetworkPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(50, 0, 50, 0);

  ListWidget *list = new ListWidget();
  list->setSpacing(30);
  // wifi + tethering buttons
#ifdef QCOM
  auto wifiBtn = new ButtonControl(tr("اعدادات الواي فاي"), tr("افتح"));
  QObject::connect(wifiBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_wifi(); });
  list->addItem(wifiBtn);

  auto tetheringBtn = new ButtonControl(tr("إعدادات الربط"), tr("افتح"));
  QObject::connect(tetheringBtn, &ButtonControl::clicked, [=]() { HardwareEon::launch_tethering(); });
  list->addItem(tetheringBtn);
#endif
  ipaddress = new LabelControl(tr("عنوان IP"), "");
  list->addItem(ipaddress);

  list->addItem(new HotspotOnBootToggle());

  // SSH key management
  list->addItem(new SshToggle());
  list->addItem(new SshControl());
  list->addItem(new SshLegacyToggle());

  layout->addWidget(list);
  layout->addStretch(1);
}

void C2NetworkPanel::showEvent(QShowEvent *event) {
  ipaddress->setText(getIPAddress());
}

QString C2NetworkPanel::getIPAddress() {
  std::string result = util::check_output("ifconfig wlan0");
  if (result.empty()) return "";

  const std::string inetaddrr = "inet addr:";
  std::string::size_type begin = result.find(inetaddrr);
  if (begin == std::string::npos) return "";

  begin += inetaddrr.length();
  std::string::size_type end = result.find(' ', begin);
  if (end == std::string::npos) return "";

  return result.substr(begin, end - begin).c_str();
}

QWidget *network_panel(QWidget *parent) {
#ifdef QCOM
  return new C2NetworkPanel(parent);
#else
  return new Networking(parent);
#endif
}

UIPanel::UIPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new AutoShutdown());
  layout->addWidget(new ForceShutdown());
  layout->addWidget(new VolumeControl());
  layout->addWidget(new BrightnessControl());
  layout->addWidget(new AutoScreenOff());
  layout->addWidget(new BrightnessOffControl());
  layout->addWidget(new GetOffAlert());
  layout->addWidget(new BatteryChargingControlToggle());
  layout->addWidget(new ChargingMin());
  layout->addWidget(new ChargingMax());
  layout->addWidget(new DrivingRecordToggle());
  layout->addWidget(new RecordCount());
  layout->addWidget(new RecordQuality());
  const char* record_del = "rm -f /storage/emulated/0/videos/*";
  auto recorddelbtn = new ButtonControl(tr("حذف كافة الملفات المسجلة"), tr("شغل"));
  QObject::connect(recorddelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm(tr("احذف جميع الملفات المسجلة المحفوظة. هل تريد المتابعة؟"), this)){
      std::system(record_del);
    }
  });
  layout->addWidget(recorddelbtn);
  const char* realdata_del = "rm -rf /data/media/0/realdata/*";
  auto realdatadelbtn = new ButtonControl(tr("حذف كافة سجلات القيادة"), tr("شغل"));
  QObject::connect(realdatadelbtn, &ButtonControl::clicked, [=]() {
    if (ConfirmationDialog::confirm(tr("احذف جميع سجلات القيادة المحفوظة. هل تريد المتابعة؟"), this)){
      std::system(realdata_del);
    }
  });
  layout->addWidget(realdatadelbtn);
  layout->addWidget(new MonitoringMode());
  layout->addWidget(new MonitorEyesThreshold());
  layout->addWidget(new NormalEyesThreshold());
  layout->addWidget(new BlinkThreshold());
  layout->addWidget(new OPKRNaviSelect());
  layout->addWidget(new RunNaviOnBootToggle());
  layout->addWidget(new OPKRServerSelect());
  layout->addWidget(new OPKRServerAPI());
  layout->addWidget(new MapboxEnabledToggle());
  layout->addWidget(new OPKRMapboxStyle());
  layout->addWidget(new GoogleMapEnabledToggle());
  layout->addWidget(new OPKRTopTextView());
  layout->addWidget(new RPMAnimatedToggle());
  layout->addWidget(new RPMAnimatedMaxValue());
  layout->addWidget(new ShowStopLineToggle());
  layout->addWidget(new HoldForSettingToggle());
  layout->addWidget(new RTShieldToggle());
  layout->addWidget(new OSMOfflineUseToggle());

}

DrivingPanel::DrivingPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new AutoResumeToggle());
  layout->addWidget(new RESCountatStandstill());
  layout->addWidget(new CruiseGapAdjustToggle());
  layout->addWidget(new StandstillResumeAltToggle());
  layout->addWidget(new VariableCruiseToggle());
  layout->addWidget(new VariableCruiseLevel());
  layout->addWidget(new CruisemodeSelInit());
  layout->addWidget(new LaneChangeSpeed());
  layout->addWidget(new LaneChangeDelay());
  layout->addWidget(new LCTimingFactorUD());
  layout->addWidget(new LCTimingFactor());
  layout->addWidget(new LeftCurvOffset());
  layout->addWidget(new RightCurvOffset());
  layout->addWidget(new BlindSpotDetectToggle());


  layout->addWidget(new CSteerWidget());
  
  layout->addWidget(new SteerAngleCorrection());
  layout->addWidget(new TurnSteeringDisableToggle());
  layout->addWidget(new CruiseOverMaxSpeedToggle());
  layout->addWidget(new OSMEnabledToggle());
  layout->addWidget(new OSMSpeedLimitEnabledToggle());
  layout->addWidget(new StockNaviSpeedToggle());
  layout->addWidget(new SpeedLimitOffset());
  layout->addWidget(new OSMCustomSpeedLimitUD());
  layout->addWidget(new OSMCustomSpeedLimit());
  layout->addWidget(new SpeedLimitSignType());
  layout->addWidget(new CamDecelDistAdd());
  layout->addWidget(new CurvDecelSelect());
  layout->addWidget(new VCurvSpeedUD());
  layout->addWidget(new VCurvSpeed());
  layout->addWidget(new OCurvSpeedUD());
  layout->addWidget(new OCurvSpeed());
  layout->addWidget(new AutoEnabledToggle());
  layout->addWidget(new AutoEnableSpeed());
  layout->addWidget(new CruiseAutoResToggle());
  layout->addWidget(new RESChoice());
  layout->addWidget(new AutoResCondition());
  layout->addWidget(new AutoResLimitTime());
  layout->addWidget(new AutoRESDelay());
  layout->addWidget(new LaneWidth());
  layout->addWidget(new SpeedLaneWidthUD());
  layout->addWidget(new SpeedLaneWidth());
  layout->addWidget(new RoutineDriveOnToggle());
  layout->addWidget(new RoutineDriveOption());
  layout->addWidget(new CloseToRoadEdgeToggle());
  layout->addWidget(new OPKREdgeOffset());
  layout->addWidget(new ToAvoidLKASFaultToggle());
  layout->addWidget(new ToAvoidLKASFault());
  layout->addWidget(new SpeedCameraOffsetToggle());

}

DeveloperPanel::DeveloperPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new DebugUiOneToggle());
  layout->addWidget(new DebugUiTwoToggle());
  layout->addWidget(new DebugUiThreeToggle());
  layout->addWidget(new ShowErrorToggle());
  layout->addWidget(new LongLogToggle());
  layout->addWidget(new PrebuiltToggle());
  layout->addWidget(new FPTwoToggle());
  layout->addWidget(new WhitePandaSupportToggle());
  layout->addWidget(new BattLessToggle());
  layout->addWidget(new ComIssueToggle());
  layout->addWidget(new LDWSToggle());
  layout->addWidget(new GearDToggle());
  layout->addWidget(new SteerWarningFixToggle());
  layout->addWidget(new IgnoreCanErroronISGToggle());
  layout->addWidget(new FCA11MessageToggle());
  layout->addWidget(new UFCModeEnabledToggle());
  layout->addWidget(new StockLKASEnabledatDisenagedStatusToggle());
  layout->addWidget(new C2WithCommaPowerToggle());
  layout->addWidget(new JoystickModeToggle());
  layout->addWidget(new NoSmartMDPSToggle());
  layout->addWidget(new UserSpecificFeature());
  layout->addWidget(new TimeZoneSelectCombo());

  layout->addWidget(horizontal_line());
  layout->addWidget(new CarSelectCombo());

  layout->addWidget(new CPandaGroup());

}

TuningPanel::TuningPanel(QWidget *parent) : QFrame(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);

  layout->setContentsMargins(50, 0, 50, 0);
  layout->setSpacing(30);

  // OPKR
  layout->addWidget(new LabelControl(tr("〓〓〓〓〓〓〓〓【 التعديل 】〓〓〓〓〓〓〓〓"), ""));
  layout->addWidget(new CameraOffset());
  layout->addWidget(new PathOffset());
  layout->addWidget(horizontal_line());

  layout->addWidget(new SteerActuatorDelay());
  layout->addWidget(new SteerRateCost());
  layout->addWidget(new TireStiffnessFactor());
  layout->addWidget(new SteerThreshold());
  layout->addWidget(new SteerLimitTimer());

  layout->addWidget(new LiveSteerRatioToggle());
  layout->addWidget(new LiveSRPercent());
  layout->addWidget(new SRBaseControl());
  layout->addWidget(new SRMaxControl());

  layout->addWidget(horizontal_line());
  layout->addWidget(new VariableSteerMaxToggle());
  layout->addWidget(new SteerMax());
  layout->addWidget(new VariableSteerDeltaToggle());
  layout->addWidget(new SteerDeltaUp());
  layout->addWidget(new SteerDeltaDown());

  layout->addWidget(horizontal_line());
  layout->addWidget(new ToAvoidLKASFaultBeyondToggle());
  layout->addWidget(new DesiredCurvatureLimit());

  layout->addWidget(horizontal_line());

  //layout->addWidget(new LabelControl("〓〓〓〓〓〓〓〓【 تحكم 】〓〓〓〓〓〓〓〓", ""));
 // layout->addWidget(new LateralControl());
  layout->addWidget(new LiveTunePanelToggle());

  layout->addWidget(new CLateralControlGroup());
  layout->addWidget(horizontal_line());
  layout->addWidget(new CLongControlGroup());

}

void SettingsWindow::showEvent(QShowEvent *event) {
  panel_widget->setCurrentIndex(0);
  nav_btns->buttons()[0]->setChecked(true);
}

SettingsWindow::SettingsWindow(QWidget *parent) : QFrame(parent) {

  // setup two main layouts
  sidebar_widget = new QWidget;
  QVBoxLayout *sidebar_layout = new QVBoxLayout(sidebar_widget);
  sidebar_layout->setMargin(0);
  panel_widget = new QStackedWidget();
  panel_widget->setStyleSheet(R"(
    border-radius: 30px;
    background-color: #292929;
  )");

  // close button
  QPushButton *close_btn = new QPushButton("×");
  close_btn->setStyleSheet(R"(
    QPushButton {
      font-size: 140px;
      padding-bottom: 20px;
      font-weight: bold;
      border 1px grey solid;
      border-radius: 50px;
      background-color: #292929;
      font-weight: 400;
    }
    QPushButton:pressed {
      background-color: #3B3B3B;
    }
  )");
  close_btn->setFixedSize(220, 130);
  sidebar_layout->addSpacing(5);
  sidebar_layout->addWidget(close_btn, 0, Qt::AlignCenter);
  QObject::connect(close_btn, &QPushButton::clicked, this, &SettingsWindow::closeSettings);

  // setup panels
  DevicePanel *device = new DevicePanel(this);
  SoftwarePanel *software = new SoftwarePanel(this);
  QObject::connect(device, &DevicePanel::reviewTrainingGuide, this, &SettingsWindow::reviewTrainingGuide);
  QObject::connect(device, &DevicePanel::showDriverView, this, &SettingsWindow::showDriverView);
  QObject::connect(software, &SoftwarePanel::closeSettings, this, &SettingsWindow::closeSettings);

  QList<QPair<QString, QWidget *>> panels = {
    {tr("جهاز"), device},
    {tr("الشبكة"), network_panel(this)},
    {tr("تبديل "), new TogglesPanel(this)},
    {tr("البرنامج"), software},
    {tr("الواجهة"), new UIPanel(this)},
    {tr("القيادة"), new DrivingPanel(this)},
    {tr("التطوير"), new DeveloperPanel(this)},
    {tr("التعديل"), new TuningPanel(this)},
  };

  sidebar_layout->addSpacing(45);

#ifdef ENABLE_MAPS
  auto map_panel = new MapPanel(this);
  panels.push_back({tr("الملاحة"), map_panel});
  QObject::connect(map_panel, &MapPanel::closeSettings, this, &SettingsWindow::closeSettings);
#endif

  const int padding = panels.size() > 3 ? 0 : 15;

  nav_btns = new QButtonGroup(this);
  for (auto &[name, panel] : panels) {
    QPushButton *btn = new QPushButton(name);
    btn->setCheckable(true);
    btn->setChecked(nav_btns->buttons().size() == 0);
    btn->setStyleSheet(QString(R"(
      QPushButton {
        color: grey;
        border: none;
        background: none;
        font-size: 54px;
        font-weight: 500;
        padding-top: %1px;
        padding-bottom: %1px;
      }
      QPushButton:checked {
        color: white;
      }
      QPushButton:pressed {
        color: #ADADAD;
      }
    )").arg(padding));

    nav_btns->addButton(btn);
    sidebar_layout->addWidget(btn, 0, Qt::AlignRight);

    const int lr_margin = name != tr("الشبكة") ? 50 : 0;  // Network panel handles its own margins
    panel->setContentsMargins(lr_margin, 25, lr_margin, 25);

    ScrollView *panel_frame = new ScrollView(panel, this);
    panel_widget->addWidget(panel_frame);

    QObject::connect(btn, &QPushButton::clicked, [=, w = panel_frame]() {
      btn->setChecked(true);
      panel_widget->setCurrentWidget(w);
    });
  }
  sidebar_layout->setContentsMargins(50, 50, 100, 50);

  // main settings layout, sidebar + main panel
  QHBoxLayout *main_layout = new QHBoxLayout(this);

  sidebar_widget->setFixedWidth(500);
  main_layout->addWidget(sidebar_widget);
  main_layout->addWidget(panel_widget);

  setStyleSheet(R"(
    * {
      color: white;
      font-size: 50px;
    }
    SettingsWindow {
      background-color: black;
    }
  )");
}

void SettingsWindow::hideEvent(QHideEvent *event) {
#ifdef QCOM
  HardwareEon::close_activities();
#endif
}
