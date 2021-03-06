/*
 * Copyright (C) 2012, 2014, 2016 Igalia S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "UserAgentQuirks.h"

#include "PublicSuffix.h"
#include "URL.h"

namespace WebCore {

// When editing the quirks in this file, be sure to update
// Tools/TestWebKitAPI/Tests/WebCore/UserAgentQuirks.cpp.

// Be careful with this quirk: it's an invitation for sites to use JavaScript
// that works in Chrome that WebKit cannot handle. Prefer other quirks instead.
static bool urlRequiresChromeBrowser(const URL& url)
{
    String baseDomain = topPrivatelyControlledDomain(url.host());

    // Needed for fonts on many sites to work with WebKit.
    // https://bugs.webkit.org/show_bug.cgi?id=147296
    if (baseDomain == "typekit.net" || baseDomain == "typekit.com")
        return true;

    // Shut off Chrome ads. Avoid missing features on maps.google.com. Avoid
    // receiving a terrible fallback version of calendar.google.com. Receive a
    // fancier plus.google.com.
    if (baseDomain.startsWith("google."))
        return true;

    // Needed for YouTube 360 with WebKitGTK+ and WPE (requires ENABLE_MEDIA_SOURCE).
    if (baseDomain == "youtube.com")
        return true;

    // Slack completely blocks users with WebKitGTK+'s standard user agent.
    if (baseDomain == "slack.com")
        return true;

    return false;
}

static bool urlRequiresMacintoshPlatform(const URL& url)
{
    String baseDomain = topPrivatelyControlledDomain(url.host());

    // At least finance.yahoo.com displays a mobile version with WebKitGTK+'s standard user agent.
    if (baseDomain == "yahoo.com")
        return true;

    // taobao.com displays a mobile version with WebKitGTK+'s standard user agent.
    if (baseDomain == "taobao.com")
        return true;

    // web.whatsapp.com completely blocks users with WebKitGTK+'s standard user agent.
    if (baseDomain == "whatsapp.com")
        return true;

    return false;
}

UserAgentQuirks UserAgentQuirks::quirksForURL(const URL& url)
{
    ASSERT(!url.isNull());
    UserAgentQuirks quirks;
    if (urlRequiresChromeBrowser(url))
        quirks.add(UserAgentQuirks::NeedsChromeBrowser);
    if (urlRequiresMacintoshPlatform(url))
        quirks.add(UserAgentQuirks::NeedsMacintoshPlatform);
    return quirks;
}

String UserAgentQuirks::stringForQuirk(UserAgentQuirk quirk)
{
    switch (quirk) {
    case NeedsChromeBrowser:
        // Get versions from https://chromium.googlesource.com/chromium/src.git
        return ASCIILiteral("Chrome/56.0.2891.4");
    case NeedsMacintoshPlatform:
        return ASCIILiteral("Macintosh; Intel Mac OS X 10_12");
    case NumUserAgentQuirks:
    default:
        ASSERT_NOT_REACHED();
    }
    return ASCIILiteral("");
}

}
