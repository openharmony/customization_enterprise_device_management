/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMMON_NATIVE_INCLUDE_WALL_PAPER_PARAM_H
#define COMMON_NATIVE_INCLUDE_WALL_PAPER_PARAM_H

namespace OHOS {
namespace EDM {
struct WallPaperParam {
    bool isHomeWallPaper = false;
    int32_t fd = 0;
};
}
}
#endif // COMMON_NATIVE_INCLUDE_WALL_PAPER_PARAM_H