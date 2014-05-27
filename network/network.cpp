/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "network.h"

namespace network {

bool isSeqIdMoreRecent(seq_id_t seqId1, seq_id_t seqId2)
{
    // Inspired by
    // http://gafferongames.com/networking-for-game-programmers/reliability-and-flow-control/
    return
        (( seqId1 > seqId2 ) &&
        ( seqId1 - seqId2 <= MAX_SEQ_ID/2 ))
        ||
        (( seqId2 > seqId1 ) &&
        ( seqId2 - seqId1  > MAX_SEQ_ID/2 ));
}

seq_id_t seqIdDifference(seq_id_t seqId1, seq_id_t seqId2) {
    seq_id_t diff;
    if(isSeqIdMoreRecent(seqId1, seqId2)) {
        diff = seqId1 - seqId2;
    }
    else {
        diff = seqId2 - seqId1;
    }
    return diff;
}

bool isSeqIdAcknowledged(ack_mask_t ackMask, seq_id_t remoteSeqId, seq_id_t seqId) {
    if(seqId == remoteSeqId) {
        return true;
    }
    if(isSeqIdMoreRecent(seqId, remoteSeqId)) {
        return false;
    }
    seq_id_t diff = seqIdDifference(seqId, remoteSeqId);
    if(diff >= NUM_ACKS) {
        return false;
    }
    return ackMask & (1 << (diff-1));
}

}
