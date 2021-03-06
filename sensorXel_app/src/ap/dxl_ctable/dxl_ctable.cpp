/*
 * dxl_ctable.cpp
 *
 *  Created on: 2017. 7. 18.
 *      Author: baram
 */



#include "ap.h"
#include "util.h"
#include "xels/xels.h"




void updateVersion(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
void updateDxlId(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
void updateDxlBaud(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
void updateMillis(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
void updateXelHeader(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
void updateXelData(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);


ctable_attribute_t ctable_sensor[] =
{
//addr                            len  count
  { P_CONST_MODEL_NUMBER,           2,     1, _ATTR_RD           ,  DXL_MODEL_NUMBER, _UPDATE_NONE    , _DEF_TYPE_U16,    NULL },
  { P_CONST_MODEL_INFO,             4,     1, _ATTR_RD           ,                 0, _UPDATE_NONE    , _DEF_TYPE_U32,    NULL },
  { P_CONST_FW_VERSION,             1,     1, _ATTR_RD           ,                 1, _UPDATE_NONE    , _DEF_TYPE_U08,    updateVersion },
  { P_EEP_ID,                       1,     1, _ATTR_RD | _ATTR_WR,       DXL_INIT_ID, _UPDATE_SETUP   , _DEF_TYPE_U08,    updateDxlId },
  { P_EEP_DXL_BAUDRATE,             1,     1, _ATTR_RD | _ATTR_WR,     DXL_INIT_BAUD, _UPDATE_SETUP   , _DEF_TYPE_U08,    updateDxlBaud },
  { P_MILLIS,                       4,     1, _ATTR_RD           ,                 0, _UPDATE_NONE    , _DEF_TYPE_U32,    updateMillis },

  { P_XEL_HEADER_DATA_TYPE,         1,     1, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_SETUP   , _DEF_TYPE_U08,    updateXelHeader },
  { P_XEL_HEADER_DATA_INTERVAL,     4,     1, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U32,    updateXelHeader },
  { P_XEL_HEADER_DATA_NAME,         1,    32, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U08,    updateXelHeader },
  { P_XEL_HEADER_DATA_DIRECTION,    1,     1, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U08,    updateXelHeader },
  { P_XEL_HEADER_DATA_ADDR,         2,     1, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U08,    updateXelHeader },
  { P_XEL_HEADER_DATA_LENGTH,       1,     1, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U08,    updateXelHeader },

  { P_XEL_DATA,                     1,   128, _ATTR_RD | _ATTR_WR,                 0, _UPDATE_NONE    , _DEF_TYPE_U08,    updateXelData },





  { 0xFFFF,  1, 0, 0, 0, 0, 0, NULL }
};








void dxlCtableInit(void)
{
  if (eepromReadByte(EEP_ADDR_CHECK_AA) != 0xAA || eepromReadByte(EEP_ADDR_CHECK_55) != 0x55)
  {
    eepromWriteByte(EEP_ADDR_CHECK_AA, 0xAA);
    eepromWriteByte(EEP_ADDR_CHECK_55, 0x55);

    eepromWriteByte(EEP_ADDR_ID, DXL_INIT_ID);
    eepromWriteByte(EEP_ADDR_BAUD, DXL_INIT_BAUD);
  }

  ctableInit(&p_ap->ctable, NULL, 1024, ctable_sensor);
}

void updateVersion(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{
  if (mode == _UPDATE_RD)
  {
    switch(addr)
    {
      case P_CONST_FW_VERSION:
        p_data[0] = p_ap->firmware_version;
        break;
    }
  }

  if (mode == _UPDATE_WR)
  {
    switch(addr)
    {
      case P_CONST_FW_VERSION:
        p_data[0] = p_ap->firmware_version;
        break;
    }
  }
}

void updateDxlId(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{

  if (mode == _UPDATE_SETUP)
  {
    if (addr == P_EEP_ID)
    {
      p_data[0] = eepromReadByte(EEP_ADDR_ID);
      p_ap->p_dxl_motor->id = p_data[0];
      dxlSetId(&p_ap->p_dxl_motor->node, p_ap->p_dxl_motor->id);
    }
  }

  if (mode == _UPDATE_RD)
  {
    if (addr == P_EEP_ID)
    {
      p_data[0] = eepromReadByte(EEP_ADDR_ID);
      p_ap->p_dxl_motor->id = p_data[0];
      dxlSetId(&p_ap->p_dxl_motor->node, p_ap->p_dxl_motor->id);
    }
  }

  if (mode == _UPDATE_WR)
  {
    if (addr == P_EEP_ID)
    {
      eepromWriteByte(EEP_ADDR_ID, p_data[0]);
      p_ap->p_dxl_motor->id = p_data[0];
      dxlSetId(&p_ap->p_dxl_motor->node, p_ap->p_dxl_motor->id);
    }
  }
}

static uint32_t getDxlBaud(uint8_t baud_index)
{
  uint32_t baud = 0;


  switch(baud_index)
  {
    case _DEF_DXL_BAUD_9600:
      baud = 9600;
      break;

    case _DEF_DXL_BAUD_57600:
      baud = 57600;
      break;

    case _DEF_DXL_BAUD_115200:
      baud = 115200;
      break;

    case _DEF_DXL_BAUD_1000000:
      baud = 1000000;
      break;

    case _DEF_DXL_BAUD_2000000:
      baud = 2000000;
      break;

    case _DEF_DXL_BAUD_3000000:
      baud = 3000000;
      break;

    case _DEF_DXL_BAUD_4000000:
      baud = 4000000;
      break;

    case _DEF_DXL_BAUD_4500000:
      baud = 4500000;
      break;

    default:
      baud = 1000000;
      break;
  }

  return baud;
}

void updateDxlBaud(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{
  uint32_t baud = 0;


  if (mode == _UPDATE_SETUP)
  {
    p_data[0] = eepromReadByte(EEP_ADDR_BAUD);
    baud = getDxlBaud(p_data[0]);

    if (baud > 0)
    {
      eepromWriteByte(EEP_ADDR_BAUD, p_data[0]);

      p_ap->p_dxl_motor->baud = baud;
      dxlOpenPort(&p_ap->p_dxl_motor->node, p_ap->p_dxl_motor->ch, p_ap->p_dxl_motor->baud);
    }
  }

  if (mode == _UPDATE_RD)
  {
    p_data[0] = eepromReadByte(EEP_ADDR_BAUD);
  }

  if (mode == _UPDATE_WR)
  {
    baud = getDxlBaud(p_data[0]);

    if (baud > 0)
    {
      eepromWriteByte(EEP_ADDR_BAUD, p_data[0]);

      p_ap->p_dxl_motor->baud = baud;
      dxlOpenPort(&p_ap->p_dxl_motor->node, p_ap->p_dxl_motor->ch, p_ap->p_dxl_motor->baud);
    }
  }
}

void updateMillis(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{
  data_t value;


  if (mode == _UPDATE_RD)
  {
    value.u32Data = millis();
    memcpy(p_data, &value.u8Data[update_addr], update_length);
  }
}

void updateXelHeader(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{
  static XelNetwork::XelHeader_t *p_xel_header;
  uint8_t *p_value;
  uint32_t index;

  p_xel_header = xelsGetHeader(0);

  if (mode == _UPDATE_SETUP)
  {
    if (addr == P_XEL_HEADER_DATA_TYPE)
    {
      uint8_t check_sum = 0;

      for (int i=EEP_ADDR_XEL_HEADER_1; i<EEP_ADDR_XEL_HEADER_1_CHECKSUM; i++)
      {
        check_sum ^= eepromReadByte(i);
      }

      p_value = (uint8_t *)p_xel_header;

      if (   check_sum != eepromReadByte(EEP_ADDR_XEL_HEADER_1_CHECKSUM)
          || eepromReadByte(EEP_ADDR_XEL_HEADER_1_CHECK_AA) != 0xAA
          || eepromReadByte(EEP_ADDR_XEL_HEADER_1_CHECK_55) != 0x55)
      {
        check_sum = 0;

        index = 0;
        for (int i=EEP_ADDR_XEL_HEADER_1; i<EEP_ADDR_XEL_HEADER_1_CHECKSUM; i++)
        {
          eepromWriteByte(i, p_value[index]);

          check_sum ^= p_value[index];
          index++;
        }

        eepromWriteByte(EEP_ADDR_XEL_HEADER_1_CHECK_AA, 0xAA);
        eepromWriteByte(EEP_ADDR_XEL_HEADER_1_CHECK_55, 0x55);
        eepromWriteByte(EEP_ADDR_XEL_HEADER_1_CHECKSUM, check_sum);
      }
      else
      {
        index = 0;
        for (int i=EEP_ADDR_XEL_HEADER_1; i<EEP_ADDR_XEL_HEADER_1_CHECKSUM; i++)
        {
          p_value[index] = eepromReadByte(i);
          index++;
        }
      }
    }
  }


  if (mode == _UPDATE_RD)
  {
    switch(addr)
    {
      case P_XEL_HEADER_DATA_TYPE:
        p_value = (uint8_t *)&p_xel_header->data_type;
        memcpy(p_data, &p_value[update_addr], update_length);
        break;

      case P_XEL_HEADER_DATA_INTERVAL:
        p_value = (uint8_t *)&p_xel_header->data_get_interval_hz;
        memcpy(p_data, &p_value[update_addr], update_length);
        break;

      case P_XEL_HEADER_DATA_NAME:
        p_value = (uint8_t *)&p_xel_header->data_name[0];
        memcpy(p_data, &p_value[update_addr], update_length);
        break;

      case P_XEL_HEADER_DATA_DIRECTION:
        p_value = (uint8_t *)&p_xel_header->data_direction;
        memcpy(p_data, &p_value[update_addr], update_length);
        break;

      case P_XEL_HEADER_DATA_ADDR:
        p_value = (uint8_t *)&p_xel_header->data_addr;
        memcpy(p_data, &p_value[update_addr], update_length);
        break;

      case P_XEL_HEADER_DATA_LENGTH:
        p_xel_header->data_length = xelsGetDataTypeLength(p_xel_header->data_type);
        p_value = (uint8_t *)&p_xel_header->data_length;
        memcpy(p_data, &p_value[update_addr], update_length);
        break;
    }
  }

  if (mode == _UPDATE_WR)
  {
    switch(addr)
    {
      case P_XEL_HEADER_DATA_TYPE:
        p_value = (uint8_t *)&p_xel_header->data_type;
        memcpy(&p_value[update_addr], p_data, update_length);
        break;

      case P_XEL_HEADER_DATA_INTERVAL:
        p_value = (uint8_t *)&p_xel_header->data_get_interval_hz;
        memcpy(&p_value[update_addr], p_data, update_length);
        break;

      case P_XEL_HEADER_DATA_NAME:
        p_value = (uint8_t *)&p_xel_header->data_name[0];
        memcpy(&p_value[update_addr], p_data, update_length);
        break;

      case P_XEL_HEADER_DATA_DIRECTION:
        p_value = (uint8_t *)&p_xel_header->data_direction;
        memcpy(&p_value[update_addr], p_data, update_length);
        break;
    }

    for (int i=0; i<update_length; i++)
    {
      eepromWriteByte(EEP_ADDR_XEL_HEADER_1 + (addr + update_addr) - P_XEL_HEADER_DATA_TYPE + i, p_data[i]);
    }
    uint8_t check_sum = 0;

    for (int i=EEP_ADDR_XEL_HEADER_1; i<EEP_ADDR_XEL_HEADER_1_CHECKSUM; i++)
    {
      check_sum ^= eepromReadByte(i);
    }
    eepromWriteByte(EEP_ADDR_XEL_HEADER_1_CHECKSUM, check_sum);
  }
}

void updateXelData(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length)
{
  if (mode == _UPDATE_RD)
  {
    xelsReadCallback(0, update_addr, p_data, update_length);
  }

  if (mode == _UPDATE_WR)
  {
    xelsWriteCallback(0, update_addr, p_data, update_length);
  }
}
