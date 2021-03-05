# LabCA - EPICS/Channel Access Interface for Scilab and Matlab

Till Straumann \<strauman/at/slac.stanford.edu\>, 2002-2021. Last
Modified: 2021/03/04.

Here comes yet another [CA/EPICS](http://www.aps.anl.gov/epics) interface.
This one builds on top of a modified (thread-safe) version of
[libezca](http://www.aps.anl.gov/epics/extensions/ezca/index.php). This
modified version comes with labCA.

The repository can be found [here](https://www.github.com/till-s/epics-labca).

### Recent Changes 

2021/03/04:

:   [labCA 3.8.0](https://github.com/till-s/epics-labca/releases/tag/labca_3_8_0)
    released.
    This release supports scilab-6 (new scilab API -- older versions are
    no longer supported).

    Also, some work-arounds for matlab-2020b have been added. Note that
    using 2020b still requires special attention (see
    [ReleaseNotes](https://github.com/till-s/epics-labca/blob/labca_3_8_0/ReleaseNotes))

	Also: labCA is now maintained on [github](https://www.github.com/till-s/epics-labca).

2018/03/04:

:   [labCA 3.7.2](https://www.slac.stanford.edu/~strauman/labca/labca_3_7_2.tgz) released.
    This works around matlab 2017b crashing during initialization of labCA.

    *Note:* This is a bugfix release, replacing 3.7.1 which has been
    retired (the tarball was incomplete, sorry). 3.7.2 also addresses
    issues in a windows environment without bash and other common unix
    tools.

    Read the [ReleaseNotes](https://raw.githubusercontent.com/till-s/epics-labca/master/ReleaseNotes)
    regarding a problem with matlab 2020b hanging during labCA initialization.

### The key features of labCA

-   Supports [scilab](http://www.scilab.org/) and matlab.
-   [EPICS](http://www.aps.anl.gov/epics/index.php) 3.14, 3.15 and 7 supported;
    (works with 3.13, too *but 3.13 support is dropped from
    labca version 3 on*).
-   Read multiple PVs at once (pass column vector of PV names).
-   On-line help available.
-   Abort CA transfers by hitting Ctrl-C (experimental).
-   Control over CA data type for transfer (native is default).
-   Read and write enums / menues as strings or numerics.
-   Automatic error handling; checks for CA severity when reading VAL
    (prints message and returns NaN).
-   Optionally, obtain EPICS timestamps along with data.
-   Interface to most ezca calls implemented.
-   Tuning of timout parameters possible.
-   Uses EPICS makefile system.

### Supported Versions / Platforms

LabCA has been tested (note that not all combinations but usually the
latest version of each component have been tested with the latest
version of LabCA) with/on.

#### Operating Systems

-   linux-x86
-   linux-x86\_64
-   linux-ppc
-   solaris-sparc-gnu
-   solaris-sparc, solaris-sparc64
-   win32-x86
-   windows-x64

#### EPICS Versions

-   7.0.4.1
-   3.15
-   3.14 (latest version uses 3.14.12.3)
-   3.13 (Note that 3.13 support is deprecated)

#### Applications

##### Scilab

-   Scilab-6.1, Scilab-5.5.0 (docs build again)
-   scilab-5.3.3 (note that the documentation does not build under
    5.3.3 - but it did under 5.2.2. A bug report has been filed but
    you don't need to build documentation anyways unless you change
    `manual.tex`).
-   Scilab-5.2, Scilab-4.1, Scilab-4.0, Scilab-3.1.1,
    Scilab-3.0, Scilab-2.7 (2.7.2 under windoze).

##### Matlab

-   Matlab 2020b requires work-arounds (see
    [ReleaseNotes](https://raw.githubusercontent.com/till-s/epics-labca/master/ReleaseNotes))
-   Matlab 2019b, 2020a
-   Matlab 7 (beta), 7.0, 7.1, 7.2, 7.3, 7.4, 7.10 (R201a), 7.12
    (R2011a).
    Note that LabCA binaries built for 7.3 are not binary compatible
    with earlier versions (but I have tried them on 7.4), R2012a,
    R2014a.
-   Matlab 6.5

### Download

-   Download from [github](https://www.github.com/till-s/epics-labca).
-   Read the latest manual ([pdf](manual.pdf) or
    [html](manual/index.html)) including examples and build
    instructions.

### Known Issues

[This document](https://www.slac.stanford.edu/~strauman/labca/epicsInstall31482.pdf) by Jim Sebek describes problems
encountered when building labca-3.1 on MS windows vista and how they
were addressed.

### License

labCA is released under the
[EPICS Open License](http://www.aps.anl.gov/epics/license/index.php).

## Old ChangeLog
2017/08/08:

:   **labCA 3.5.1** released. lcaGet of DBF\_LONG in
    native format resulted in an intermediate conversion to float and
    loss of precision (thanks to J. Vasquez for reporting).

    To work around this bug it is possible to request such values
    explicity as DBF\_DOUBLE (see lcaGet documentation).

2015/09/08:

:   **labCA 3.5.0** released. A few minor fixes to build
    under base-3.15 (thanks to S. Gierman for reporting).

    Also, there is now a new call:

    -   `lcaGetEnumStrings` which allows you to retrieve the possible
        values of an enum-type PV.

2015/03/12:
:   **labCA 3.4.2** released. A few minor fixes required
    to build under matlab 2014a. Thanks to Matthew Furseman for
    contributing the respective patch.

2013/06/25:
:   **labCA 3.4.1** released. Still no new code. Just a
    bugfix in the build system which only affects builds for scilab
    under windows. Thanks to Takashi Obina for reporting and suggesting
    a fix.

2012/01/17:
:   **labCA 3.4** released. This release contains no new
    code or bugfixes (I haven't received any reports since 3.3) but
    addresses the build system, mostly under windows. It should now be
    possible to build labCA under a non-cygwin (i.e., plain
    `command.com`) environment. Note that blanks in paths still cause
    grief. It's best to avoid them; for work-arounds consult the
    respective [README](https://raw.githubusercontent.com/till-s/epics-labca/master/README.win32pathswithblanks)
    in the distribution.
:   Tested under windows-64bit.

2010/06/07:

:   **labCA 3.3** released. This release implements a few
    minor improvements:

    -   `lcaGetUnits()`, `lcaGet_xxx_Limits()`, `lcaGetPrecision()` and
        `lcaGetStatus()` no longer transfer the full element count of an
        array. This speeds up these calls when working with a slow
        connection and it leaves these calls unaffected by
        `EPICS_CA_MAX_ARRAY_BYTES`.
    -   added support for scilab-5
    -   added help files for scilab-5.

    3.3. also contains the additions already present in 3.2_beta, of
    course (there never was a 3.2 release):

:   -   Added entry points to read warning- and alarm-limits:
        `lcaGetWarnLimits()`, `lcaGetAlarmLimits()`.

    This is a *source-only* release, sorry.

2007/11/14:
:   Jim Sebek contributed a [document addressing problems and
    work-arounds](https://www.slac.stanford.edu/~strauman/labca/epicsInstall31482.pdf)
    for building EPICS-3.14.8.2 and
    LABCA-3.1 on the WIN32 VISTA platform.

2007/10/13:

:   **labca-3.1** released. This is a bugfix release. Greg Portmann
    discovered a memory leak in `lcaGetNoWait()`. If you frequently
    use this routine then you should upgrade. 3.1 also plugs memory
    leaks in the scilab interface but these are less critical (memory is
    only leaked in some cases where errors are reported but not during
    error-free operation).

    Note that for the moment 3.1 is a source-code only release. I
    didn't have time to build and thoroughly test on all platforms,
    sorry. However, I believe it is desirable to get the fix out
    there...

2007/6/11:
:   **labca-3.0.beta** released. This is a 'new feature' release:
    -   labca no longer pends for CA activity in multiples of the labca
        timeout value. We now make full use of multi-threaded CA and
        return as soon as a labca command completes.
    -   added `lcaNewMonitorWait()` call which blocks execution until
        all requested monitors have new data (or a timeout expires).
    -   more consistent and informative error handling. All errors now
        can be handled with the `try-catch-end` and `lasterror`
        commands.

:   more details can be found in the ChangeLog file.

2007/6/11:
:   **labca-2.1** released. This is a bugfix release:
    -   lcaPutNoWait() didn't flush CA queue.
    -   use `mexLock()` to prevent labca from being unloaded from memory
        (matlab `clear all` would crash).
    -   64-bit portability fix
    -   scilab got stuck during initialization in non-windowing mode.

2006/4/18:
:   **labca-2.1.beta** released:
    -   *PATH and `LD_LIBRARY_PATH` settings have slightly
        changed* (see below and/or manual).
        *Update your startup scripts etc. accordingly*.
    -   Ctrl-C handling should now be more stable and more portable.
        Platform-specific hacks have been abandoned in favor of (alas
        undocumented) hooks into matlab and scilab that poll for a
        pending Ctrl-C keypress.
        *Older versions of labca don't work correctly with matlab-7.2
        on win32.*
    -   Fixed a problem with monitors that occurred when an IOC was
        rebooted.
    -   Consult `ChangeLog` for more details.

2004/6/23:

:   **labca-2.0.alpha** released. The following new features were added:

    -   `lcaClear()` allows you to clear/disconnect channels. Unconnected
        channels are automatically cleared to avoid annoying messages by
        3.14 background threads. Adding this feature involved some
        additions to EZCA and therefore the version number was changed.
    -   `lcaSetMonitor()` and `lcaNewMonitorValue()` were added (providing
        EZCA monitor functionality). Monitors are especially handy with
        EPICS 3.14 since data is automatically retrieved by background
        threads. matlab/scilab can process in the foreground while
        waiting for monitored data to change. Labca can be polled for
        new data with the lcaNewMonitorValue() call.

2004/4/2:

:   **labca-1.5.beta** released.

    This release enables standard error handling in matlab. Prior
    releases didn't throw errors because this would abort matlab7 on
    linux. The cause for this has now been discovered and is described
    in the ReleaseNotes.

2004/2/25:
:   **labca-1.4.beta released**; this release adds a matlab implementation
    to `lcaGetStatus()`. No other changes.


