/*
 * Common code for atom read methods:
 *
 * ReadWord
 * ReadDword
 * ReadQword
 *
 */
    if ( data->read_access )
    {
        if ( (data->buffer_ptr + sizeof(ret_val)) <= data->buffer_count )
        {
            memcpy(&ret_val,data->buffer+data->buffer_ptr,sizeof(ret_val));
            data->buffer_ptr += sizeof(ret_val);

        }
        else
        {
            error = SeqIO_Read( _this, NULL, &ret_val, sizeof(ret_val));
        }

        if ( data->byte_reverse )
        {
            SwapByte(&ret_val,sizeof(ret_val));
        }
    }
    else
    {
        error = errACCESS_DENIED;
    }
