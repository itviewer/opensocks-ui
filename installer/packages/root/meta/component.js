function Component()
{
    component.setUpdateAvailable(false);

    if (systemInfo.kernelType === "darwin") {
        installer.setValue("TargetDir", installer.value("HomeDir") + "/");
    }
    // end with "/" in config.xml
    var targetDir = installer.value("TargetDir");
    var uninstaller = installer.value("MaintenanceToolName");

    if (systemInfo.kernelType === "linux") {
        installer.setValue("TargetDir", targetDir + "opensocks");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller;

    } else if (systemInfo.kernelType === "winnt") {
        installer.setValue("TargetDir", targetDir + "OpenSocks");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller + ".exe";
    } else if (systemInfo.kernelType === "darwin") {
        installer.setValue("TargetDir", targetDir + "OpenSocks");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller + ".app/Contents/MacOS/uninstall";
    }

    if (installer.isInstaller()) {
        if (installer.fileExists(uninstaller)) {
            installer.execute(uninstaller);
        }

        // request when component really installing
        // component.addStopProcessForUpdateRequest("vpnui");
    }

    if (systemInfo.kernelType === "darwin") {
        component.addStopProcessForUpdateRequest("OpenSocks");
    } else {
        // kill self when install and uninstall
        component.addStopProcessForUpdateRequest("opensocks-ui");
    }
}

Component.prototype.createOperations = function()
{
    if (systemInfo.kernelType === "linux") {
        installer.gainAdminRights()

        // default call createOperationsForArchive and then createOperationsForPath
        // The default implementation is recursively creating Copy and Mkdir operations for all files and folders within path.
        component.createOperations();

        // will be auto removed on uninstall
        component.addOperation("Copy", "@TargetDir@/opensocks-ui.desktop", "/usr/share/applications/opensocks-ui.desktop");
    } else if (systemInfo.kernelType === "winnt") {
        component.createOperations();

        //开始菜单快捷方式
        component.addOperation("CreateShortcut",
                               "@TargetDir@/opensocks-ui.exe",
                               "@StartMenuDir@/OpenSocks.lnk",
                               "workingDirectory=@TargetDir@");

        //桌面快捷方式
        component.addOperation("CreateShortcut",
                               "@TargetDir@/opensocks-ui.exe",
                               "@DesktopDir@/OpenSocks.lnk",
                               "workingDirectory=@TargetDir@");

    } else if (systemInfo.kernelType === "darwin") {
        component.createOperations();
        component.addOperation("CreateLink", "@ApplicationsDir@/OpenSocks.app", "@TargetDir@/OpenSocks.app");
    }
}

