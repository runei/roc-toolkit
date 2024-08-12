/*
 * Copyright (c) 2024 Roc Streaming authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "roc_audio/vorbis_encoder.h"
#include "roc_core/panic.h"

namespace roc {
namespace audio {

VorbisEncoder::VorbisEncoder(const SampleSpec& sample_spec)
    : initialized_(false)
    , frame_data_(NULL)
    , frame_size_(0) {
    vorbis_info_init(&vorbis_info_);

    const int num_channels = static_cast<int>(sample_spec.num_channels());
    const int sample_rate = static_cast<int>(sample_spec.sample_rate());
    int ret = vorbis_encode_init_vbr(&vorbis_info_, num_channels, sample_rate, 0.0f);
    if (ret != 0) {
        roc_panic("vorbis encoder: failed to initialize vorbis encoder");
    }
    vorbis_comment_init(&vorbis_comment_);

    ret = vorbis_analysis_init(&vorbis_dsp_, &vorbis_info_);
    if (ret != 0) {
        roc_panic("vorbis encoder: failed to initialize vorbis dsp");
    }

    ret = vorbis_block_init(&vorbis_dsp_, &vorbis_block_);
    if (ret != 0) {
        roc_panic("vorbis encoder: failed to initialize vorbis block");
    }

    ogg_stream_init(&ogg_stream_, 0);
    initialized_ = true;
}

VorbisEncoder::~VorbisEncoder() {
    if (initialized_) {
        ogg_stream_clear(&ogg_stream_);
        vorbis_block_clear(&vorbis_block_);
        vorbis_dsp_clear(&vorbis_dsp_);
        vorbis_comment_clear(&vorbis_comment_);
        vorbis_info_clear(&vorbis_info_);
    }
}

status::StatusCode VorbisEncoder::init_status() const {
    return initialized_ ? status::StatusOK : status::StatusAbort;
}

size_t VorbisEncoder::encoded_byte_count(size_t num_samples) const {
    if (!initialized_) {
        return 0;
    }

    if (vorbis_info_.rate <= 0 || vorbis_info_.channels <= 0
        || vorbis_info_.bitrate_nominal <= 0) {
        roc_panic("vorbis encoder: vorbis_info structure has invalid values");
        return 0;
    }

    // Average number of bits used per sample, per channel.
    const size_t bit_width = static_cast<size_t>(
        (vorbis_info_.bitrate_nominal / vorbis_info_.rate) / vorbis_info_.channels);

    const size_t encoded_bytes = (num_samples * bit_width + 7) / 8;

    return encoded_bytes;
}

void VorbisEncoder::begin_frame(void* frame_data, size_t frame_size) {
    roc_panic_if_not(frame_data);
    if (frame_data_) {
        roc_panic("vorbis encoder: unpaired begin/end");
    }
    frame_data_ = frame_data;
    frame_size_ = frame_size;
}

size_t VorbisEncoder::write_samples(const sample_t* samples, size_t n_samples) {
    roc_panic("TODO");
    return 0;
}

void VorbisEncoder::end_frame() {
    roc_panic("TODO");
}

} // namespace audio
} // namespace roc
