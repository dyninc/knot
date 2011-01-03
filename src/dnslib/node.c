#include <malloc.h>

#include "node.h"
#include "common.h"
#include "rrset.h"

//void print_node(void *key, void *val)
//{
//	dnslib_rrset_t *rrset = (dnslib_node_t*) val;
//	int *key_i = (int*)key;
//	printf("key %d\n", key_i);
//	printf("%d\n", rrset->type);
//}

static int compare_rrset_types(void *key1, void *key2)
{
	return (*((uint16_t *)key1) == *((uint16_t *)key2) ?
	        0 : *((uint16_t *)key1) < *((uint16_t *)key2) ? -1 : 1);
}

dnslib_node_t *dnslib_node_new(dnslib_dname_t *owner, dnslib_node_t *parent)
{
	dnslib_node_t *ret = malloc(sizeof(dnslib_node_t));
	if (ret == NULL) {
		ERR_ALLOC_FAILED;
		return NULL;
	}

	ret->owner = owner;
	ret->parent = parent;
	ret->next = NULL;
	ret->rrsets = skip_create_list(compare_rrset_types);

	ret->avl.avl_left = NULL;
	ret->avl.avl_right = NULL;
	ret->avl.avl_height = 0;

	return ret;
}

int dnslib_node_add_rrset(dnslib_node_t *node, dnslib_rrset_t *rrset)
{
	if ((skip_insert(node->rrsets, 
			 (void *)&rrset->type, (void *)rrset, NULL)) != 0) {
		return -2;
	}

	return 0;
}

const dnslib_rrset_t *dnslib_node_rrset(const dnslib_node_t *node,
                                            uint16_t type)
{
	return (const dnslib_rrset_t *)skip_find(node->rrsets, (void *)&type);
}

dnslib_rrset_t *dnslib_node_get_rrset(dnslib_node_t *node, uint16_t type)
{
	return (dnslib_rrset_t *)skip_find(node->rrsets, (void *)&type);
}

const dnslib_node_t *dnslib_node_parent(const dnslib_node_t *node)
{
	return node->parent;
}

void dnslib_node_set_parent(dnslib_node_t *node, dnslib_node_t *parent)
{
	node->parent = parent;
}

void dnslib_free_node_rrsets(dnslib_node_t *node)
{
	const skip_node_t *skip_node =
		skip_first(node->rrsets);

	if (skip_node != NULL) {
		dnslib_rrset_free_tmp((dnslib_rrset_t **)&skip_node->value, 1);
		while ((skip_node = skip_next(skip_node)) != NULL) {
			dnslib_rrset_free_tmp((dnslib_rrset_t **)
			                      &skip_node->value,
					      1);
		}
	}
}

void dnslib_node_free_rrsets(dnslib_node_t **node, int free_rrsets)
{
	if (free_rrsets) {
		dnslib_free_node_rrsets(*node);
	}

	if ((*node)->rrsets == NULL) {
		printf("empty node\n");
	}

	if ((*node)->rrsets != NULL) {
		skip_destroy_list(&(*node)->rrsets, NULL, NULL);
	}
}

void dnslib_node_free_owner(dnslib_node_t **node)
{
	dnslib_dname_free(&(*node)->owner);
 	free(*node);
	*node = NULL;   
}

void dnslib_node_free(dnslib_node_t **node)
{
	if ((*node)->rrsets != NULL) {
		skip_destroy_list(&(*node)->rrsets, NULL, NULL);
	}

	free(*node);
	*node = NULL;
}


int dnslib_node_compare(dnslib_node_t *node1, dnslib_node_t *node2)
{
	return dnslib_dname_compare(node1->owner, node2->owner);
}

