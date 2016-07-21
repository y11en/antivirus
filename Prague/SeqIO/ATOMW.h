/*
 * Common code for atom write methods:
 *
 * WriteWord
 * WriteDword
 * WriteQword
 *
 */

    if ( data->write_access )
    {
        if ( data->byte_reverse )
        {
            SwapByte(&value,sizeof(value));
        }

        /* check available space in buffer */
        if ( (data->buffer_ptr + sizeof(value)) <= data->buffer_wsize )
        {
            /* buffer became dirty */
            data->buffer_dirty = cTRUE;

            /* copy data to buffer */
            memcpy(data->buffer+data->buffer_ptr,&value,sizeof(value));
            data->buffer_ptr += sizeof(value);

            /* update valid buffer size if necessary */
            if ( data->buffer_count < data->buffer_ptr )
            {
                data->buffer_count = data->buffer_ptr;
            }

        }
        else
        {
            /* no space in buffer - generic write function */
            error = SeqIO_Write( _this, NULL, &value, sizeof(value));
        }

    }
    else
    {
        error = errACCESS_DENIED;
    }
