# Contributing to KDisplay

 - [Logging and Debugging](#logging-and-debugging)
    - [Runtime logging](#runtime-logging)
    - [Accessing the Wayland compositor log](#accessing-the-wayland-compositor-log)
    - [Debugging with GDB](#debugging-with-gdb)
 - [Submission Guideline](#submission-guideline)
 - [Commit Message Guideline](#commit-message-guideline)
 - [Contact](#contact)

## Logging and Debugging
The first step in contributing to the project
either by providing meaningful feedback
or by sending in patches
is always the analysis of the runtime behavior of the program.

For KDisplay this means studying the debug log of
it being daemonized as a module of the KDE daemon and being run as the interactive KCM.
Additionally under normal operation the log of Disman's D-Bus backend launcher process should be
read out.
To do all that some processes must be started from different terminal windows.

### Runtime logging

#### Preparations
At first the lines

    disman*=true
    kdisplay*=true

should be added to the file `$HOME/.config/QtProject/qtlogging.ini` (create the file if it does not
exist already).
This enables much more debug log to be shown.

#### Disman D-Bus service

Disman by default starts a suitable backend as a D-Bus service
when being asked to by library consumers like KDisplay.
To get debug output from this service restart it:

        killall -9 disman_backend_launcher ; /usr/lib/libexec/disman_backend_launcher

For more information on logging Disman's D-Bus backend service see
[Disman's documentation][disman-service].

Alternatively a KDisplay executable component (KDED module or KCM)
can be started with the Disman backend being run in the component's own process
by setting the environment variable `DISMAN_IN_PROCESS` beforehand.
More on this below.

#### KDE Daemon module
The KDE Daemon (KDED) process that executes the KDisplay daemon module
can be restarted with:

        killall -9 kded5 ; kded5

This should give you already plenty of output showing how the KDisplay module is initialized
and informed about the current output configuration.
And possibly how the module tries to apply another configuration
in case the device is a laptop with its lid closed or in a docking station.

The environment variable `DISMAN_IN_PROCESS` can be set before starting the KDED process
what will output all Disman and KDisplay daemon module log lines into the same terminal window
but the synchronization with changes from other Disman consumers will only be partial.
Therefore it is not recommended in general.

#### KConfig Module (KCM)
The KDisplay KConfig Module (KCM) provides a frontend UI for users
to manipulate the current display configuration.
It can either be started from the respective entry in the KDE System Settings
or from a terminal by executing the command:

    kcmshell5 kcm_kdisplay

As with the KDED module the environment variable `DISMAN_IN_PROCESS` can be set
for printing the KCM log together with the Disman backend log in the same terminal window.
As long as no other process is manipulating the display configuration at the same time
this can be done without synchronization issues.
But in case a disman_backend_launcher process was running in the same D-Bus session
it is recommended to shut it down before that.

### Accessing the Wayland compositor log
If you debug KDisplay in a Wayland session it is often helpful to also get the debug output of the
Wayland compositor.
How this can be achieved depends on the compositor.

For *KWinFT* see the respective section in its documentation on [runtime logging][kwinft-log].

### Debugging with GDB
In case the KDE daemon or the KDisplay KCM crash, the GNU Debugger (GDB) can often provide
valuable information about the crash in the form of a backtrace.

For more information see KWinFT's [documentation about GDB][kwinft-debug-gdb].
Most of it can be applied to applications making use of Disman directly and that often in an easier
fashion.
For example for starting KDisplay directly with GDB simply issue from a terminal emulator in your
desktop session the command:

    gdb --ex r --args kcmshell5 kdisplay


## Submission Guideline
Contributions to KDisplay are very welcome but follow a strict process that is layed out in detail
in Wrapland's [contributing document][wrapland-submissions].

*Summarizing the main points:*

* Use [merge requests][merge-request] directly for smaller contributions, but create
  [issue tickets][issue] *beforehand* for [larger changes][wrapland-large-changes].
* Adhere to the [KDE Frameworks Coding Style][frameworks-style].
* Merge requests have to be posted against master or a feature branch. Commits to the stable branch
  are only cherry-picked from the master branch after some testing on the master branch.

## Commit Message Guideline
The [Conventional Commits 1.0.0][conventional-commits] specification is applied with the following
amendments:

* Only the following types are allowed:
  * build: changes to the CMake build system, dependencies or other build-related tooling
  * ci: changes to CI configuration files and scripts
  * docs: documentation only changes to overall project or code
  * feat: new feature
  * fix: bug fix
  * perf: performance improvement
  * refactor: rewrite of code logic that neither fixes a bug nor adds a feature
  * style: improvements to code style without logic change
  * test: addition of a new test or correction of an existing one
* Only the following optional scopes are allowed:
  * kcm
  * kded
  * plasmoid
* Angular's [Revert][angular-revert] and [Subject][angular-subject] policies are applied.

### Example

    fix(kcm): provide correct return value

    For function exampleFunction the return value was incorrect.
    Instead provide the correct value A by changing B to C.

### Tooling
See [Wrapland's documentation][wrapland-tooling] for available tooling.

## Contact
See [Wrapland's documentation][wrapland-contact] for contact information.

[angular-revert]: https://github.com/angular/angular/blob/3cf2005a936bec2058610b0786dd0671dae3d358/CONTRIBUTING.md#revert
[angular-subject]: https://github.com/angular/angular/blob/3cf2005a936bec2058610b0786dd0671dae3d358/CONTRIBUTING.md#subject
[conventional-commits]: https://www.conventionalcommits.org/en/v1.0.0/#specification
[disman-service]: https://gitlab.com/kwinft/disman/-/blob/master/CONTRIBUTING.md#dismans-d-bus-backend-service
[frameworks-style]: https://community.kde.org/Policies/Frameworks_Coding_Style
[issue]: https://gitlab.com/kwinft/kdisplay/-/issues
[kwinft-debug-gdb]: https://gitlab.com/kwinft/kwinft/-/blob/master/CONTRIBUTING.md#debugging-with-gdb
[kwinft-log]: https://gitlab.com/kwinft/kwinft/-/blob/master/CONTRIBUTING.md#runtime-logging
[merge-request]: https://gitlab.com/kwinft/kdisplay/-/merge_requests
[plasma-schedule]: https://community.kde.org/Schedules/Plasma_5
[wrapland-contact]: https://gitlab.com/kwinft/wrapland/-/blob/master/CONTRIBUTING.md#contact
[wrapland-large-changes]: https://gitlab.com/kwinft/wrapland/-/blob/master/CONTRIBUTING.md#issues-for-large-changes
[wrapland-submissions]: https://gitlab.com/kwinft/wrapland/-/blob/master/CONTRIBUTING.md#submission-guideline
[wrapland-tooling]: https://gitlab.com/kwinft/wrapland/-/blob/master/CONTRIBUTING.md#tooling
