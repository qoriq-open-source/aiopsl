#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "common/fsl_aiop_cmdif.h"
#include "fsl_cdma.h"
#include "io.h"
#include "cmdif_client.h"

extern void client_open_cmd(struct cmdif_desc *client, void *sync_done); 
extern void client_close_cmd(struct cmdif_desc *client); 
extern void client_sync_cmd(struct cmdif_desc *client); 
extern void client_async_cmd(struct cmdif_desc *client); 
extern void client_no_resp_cmd(struct cmdif_desc *client); 
extern void cmdif_srv_isr();

__SHRAM int received_fd = 0;
uint32_t    sync_done   = 0; 
/**< DDR, server accesses it with CDMA, 4 byte is enough */
__SHRAM struct cmdif_desc client_desc;
/**< Client descriptor that should be passed to open */
struct cmdif_dev client_dev;

#define     INVALID_AUTH_ID  1001

#pragma push
#pragma section code_type ".receivecb_func"
#pragma force_active on
#pragma function_align 256  
#pragma require_prototypes off

void app_receive_cb (void) 
{    
	uint32_t auth_id = 0;
	int test_id = received_fd % 11;
	
	cdma_read(&auth_id, fsl_os_virt_to_phys(&sync_done), 4);
	auth_id &= 0x0000FFFF;
	client_dev.auth_id = (uint16_t)auth_id;
	client_desc.dev    = &client_dev;
	
	fsl_os_print("\n Starting test number %d\n", test_id);
	switch (test_id) {
	case 0:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_open_cmd(&client_desc, &sync_done); 
		break;
	case 1:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 2:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_async_cmd(&client_desc); 
		break;
	case 3:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_no_resp_cmd(&client_desc); 
		break;
	case 4:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		((struct cmdif_dev *)client_desc.dev)->auth_id = INVALID_AUTH_ID;
		client_sync_cmd(&client_desc); 		
		break;
	case 5:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		((struct cmdif_dev *)client_desc.dev)->auth_id = INVALID_AUTH_ID;
		client_async_cmd(&client_desc);
		break;
	case 6:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_sync_cmd(&client_desc); 
		break;
	case 7:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_async_cmd(&client_desc); 
		break;
	case 8:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_no_resp_cmd(&client_desc); 
		break;		
	case 9:
		fsl_os_print("EXPECTED RESULT: PASSED\n");
		client_close_cmd(&client_desc);
		break;	
	case 10:
		fsl_os_print("EXPECTED RESULT: FAILED \n");
		fsl_os_print("client_close_cmd should fail\n");
		client_close_cmd(&client_desc);
		break;				
	default:
		break;
	}
	received_fd++;
	cmdif_srv_isr();
}

#pragma pop