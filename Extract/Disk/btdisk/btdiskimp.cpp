#include "btdiskimp.h"

tBOOL _get_bootarea_sector_by_idx(cIO* pDiskIo, tDWORD bt_idx, _disk_descr* pdisk_descr)
{
	int vtoc_sector = _vtoc_start_sector;
	tDWORD rsize;
	tDWORD idx = 0;
	tQWORD bcat_sect;

	_cd_vtoc vtoc;

	__uint8* boot_catalog = NULL;
	if (PR_FAIL( pDiskIo->heapAlloc( (tPTR*) &boot_catalog, _cd_sector_size) ))
		return cFALSE;

	while (cTRUE)
	{
		if (PR_FAIL(pDiskIo->SeekRead(&rsize, vtoc_sector * _cd_sector_size, &vtoc, sizeof(_cd_vtoc))))
			break;

		if (rsize != _cd_sector_size 
			|| vtoc.standard_identifier[0] != _validation_key_C
			|| vtoc.standard_identifier[1] != _validation_key_D)
			break;

		if (_vtoc_volume_descriptor_set_terminator == vtoc.volume_descriptor_type)
			break;

		vtoc_sector++;
		if (_vtoc_boot_record != vtoc.volume_descriptor_type)
			continue;

		_boot_record_volume_descriptor* pboot_record = (_boot_record_volume_descriptor*) &vtoc;
		bcat_sect = pboot_record->absolute_pointer_to_first_sector_of_boot_catalog;
		if (PR_FAIL(pDiskIo->SeekRead(&rsize, _cd_sector_size * bcat_sect, boot_catalog, _cd_sector_size)))
			continue;

		if (rsize != _cd_sector_size)
			break;

		_validation_entry* pValidationEntry = (_validation_entry*) boot_catalog;
		if ((_validation_key_55 != pValidationEntry->key_byte_55) || (_validation_key_aa != pValidationEntry->key_byte_aa))
		{
			pDiskIo->heapFree( boot_catalog );
			return cFALSE;
		}
		else
		{
			pdisk_descr->m_platform_id = pValidationEntry->platform_id;
			_initial_default_entry* pdef_entry = (_initial_default_entry*) (boot_catalog + sizeof(_validation_entry));

			if (pdef_entry->boot_indicator == _section_boot_inidicator)
			{
				if (bt_idx == idx)
				{
					pdisk_descr->m_load_rba = pdef_entry->load_rba;
					pdisk_descr->m_sector_count = pdef_entry->sector_count;
					
					pDiskIo->heapFree( boot_catalog );
					return cTRUE;
				}

				idx++;
			}
		}
		
		__int32 left_bytes = _cd_sector_size - sizeof(_initial_default_entry)  - sizeof(_validation_entry) - 
			sizeof(_initial_default_entry);

		while (left_bytes >= sizeof(_section_header_entry))
		{
			left_bytes -= sizeof(_section_header_entry);

			_section_header_entry* psections = (_section_header_entry*) ((__uint8*)(pValidationEntry) + sizeof(_validation_entry) + sizeof(_initial_default_entry));
			if ((psections->header_indicator != _last_section_header) && (psections->header_indicator != _section_header_indicator))
				break;

			if (psections->number_of_section_entries_following_this_header * sizeof(_section_entry) > (__uint32) left_bytes)
			{
				// invalid sections size - next sector?
				break;
			}

			left_bytes -= psections->number_of_section_entries_following_this_header * sizeof(_section_entry);

			_section_entry* pentry = (_section_entry*) ((__uint8*)psections + sizeof(_section_header_entry));

			for (int cou = 0; cou < psections->number_of_section_entries_following_this_header; cou++)
			{
				if (pentry->boot_indicator == _section_boot_inidicator)
				{
					if (bt_idx == idx)
					{
						pdisk_descr->m_load_rba = pentry->load_rba;
						pdisk_descr->m_sector_count = pentry->sector_count;
						
						pDiskIo->heapFree( boot_catalog );
						return cTRUE;
					}

					idx++;
				}
				pentry++;
			}

			if (psections->header_indicator == _last_section_header)
				break;
		}
	}
	
	pDiskIo->heapFree( boot_catalog );
	return cFALSE;
}

