#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

// C++ header
#include <ctime>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <memory>

// window header
#include <atlbase.h>
#include <WTypes.h>
#include <shellapi.h>

// base header
#include "base/callback/callback.h"
#include "base/util/string_util.h"
#include "base/util/unicode.h"
#include "base/util/string_number_conversions.h"
#include "base/memory/deleter.h"
#include "base/memory/singleton.h"
#include "base/win32/platform_string_util.h"
#include "base/win32/path_util.h"
#include "base/win32/win_util.h"
#include "base/thread/thread_manager.h"
#include "base/macros.h"
#include "base/base_types.h"
#include "base/file/file_util.h"
#include "base/time/time.h"
#include "base/framework/task.h"
#include "base/network/nio_base.h"

// shared project
#include "shared/utf8_file_util.h"
#include "shared/tool.h"
#include "shared/log.h"
#include "shared/util.h"

// duilib
#include "duilib/UIlib.h"

// json
#include "jsoncpp/include/json/json.h"

// xml
#include "tinyxml/tinyxml.h"

// tool
#include "nim_tools_http_cpp.h"

// helper
#include "shared/closure.h"

#define  APP_DATA_DIR  L"logs_monitor\\"

