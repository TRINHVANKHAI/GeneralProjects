#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _dulieu {
  double diem;
  char *hoten;
} dulieu_t;

typedef struct _tree {
  struct _tree *root;
  struct _tree *left;
  struct _tree *right;
  struct _tree *next;
  struct _tree *prev;
  double id;
  dulieu_t *sv;
} tree_t;


static tree_t *treehead;

/*
 * @brief : Find the nearest number in tree less than or equal to id
 * @root  : Root of the tree
 * @id    : Expected number
 * @return: Expected nearest floor
 */
void tree_root_init(tree_t **root) {
  tree_t *rnode = (tree_t *)malloc(sizeof(tree_t));
  if(!rnode) {
    perror("Cannot malloc root node\n");
    exit(1);
  }
  memset(rnode, 0, sizeof(tree_t));
  rnode->id = 3.141410562;
  *root = rnode;
}

/*
 * @brief  : Insert data to tree
 * @rnode  : Root of the tree
 * @id     : Expected number
 * @return : new node
 */
tree_t *tree_put_id(tree_t *rnode, double id, dulieu_t *dat) {
  tree_t *new;
  tree_t *rpnode;

  if(rnode==NULL) {
    perror("Head is null, please init\n");
    exit(1);
  }
  rpnode=rnode->right;
  if(!rpnode) {
    rnode->right = (tree_t *)malloc(sizeof(tree_t));
    memset(rnode->right, 0, sizeof(tree_t));
    rnode->right->id = id;
    rnode->right->sv = dat;
    rnode->right->root = rnode;
    return rnode->right;
  }

  for(new=rpnode;new;) {
    rpnode = new;
    new = id <= new->id ? new->left : new->right;
  }
  new = (tree_t *)malloc(sizeof(tree_t));
  if(new == NULL) {
    perror("Cannot create new node\n");
    exit(1);
  }
  new->root = rpnode;
  new->left = NULL;
  new->right = NULL;
  new->id = id;
  new->sv = dat;
  if(new->id <= new->root->id) {
    new->root->left = new;
  } else {
    new->root->right = new;
  }

  for(rpnode=new->root; rpnode->root->root; rpnode=rpnode->root)
    if(rpnode->id >= new->id) break;

  if(rpnode->id >= new->id) {
    new->next = rpnode;
    new->next->prev = new;
  } else {
    new->next = NULL;
  }

  for(rpnode=new->root; rpnode->root->root; rpnode=rpnode->root)
    if(rpnode->id < new->id) break;
  if(rpnode->id < new->id) {
    new->prev = rpnode;
    new->prev->next = new;
  } else {
    new->prev = NULL;
  }

  return new;
}

/*
 * @brief : Find the nearest number in tree less than or equal to id
 * @root  : Root of the tree
 * @id    : Expected number
 * @return: Expected nearest floor
 */
tree_t *tree_floor_lookup(tree_t *root, double id) {
  tree_t *rpnode = root->right;
  tree_t *node=rpnode;

  if(!rpnode) {
    printf("FIRST NODE IS NULL\n");
    return NULL;
  }

  for(node=rpnode;node;) {
    if ((id<node->id)&&(node->left)) {
      node=node->left;
    }
    else if ((id>node->id)&&(node->right)) {
      node=node->right;
    }
    else {
      if(node->prev)
        return node;
      else
        return NULL;
    }

    if(node->prev)
      if((node->prev->id <= id) && (node->id > id)) 
        return node->prev;

    if(node->next)
      if((node->next->id > id) && (node->id <= id))
        return node;
  }
  return NULL;
}

/*
 * @brief : Find the nearest number in tree greater than or equal to id
 * @root  : root of the tree
 * @id    : Expected number
 * @return: Expected nearest ceiling
 */
tree_t *tree_ceiling_lookup(tree_t *root, double id) {
  tree_t *rpnode = root->right;
  tree_t *node=rpnode;

  if(!rpnode) {
    printf("FIRST NODE IS NULL\n");
    return NULL;
  }

  for(node=rpnode;node;) {
    if ((id<node->id)&&(node->left)) {
      node=node->left;
    }
    else if ((id>node->id)&&(node->right)) {
      node=node->right;
    }
    else {
      if(node->next)
        return node;
      else
        return NULL;
    }

    if(node->prev)
      if((node->prev->id < id) && (node->id >= id)) 
        return node;

    if(node->next)
      if((node->next->id >= id) && (node->id < id))
        return node->next;
  }
  return NULL;
}

/*
 * @brief : Delete a node with id
 * @root  : root of the tree
 * @id    : Expected number
 * @return: 0 if success, others if failed 
 */
int tree_del_id(tree_t *rnode, double id) {
  tree_t *order;
  tree_t *node;
  tree_t *pnode;
  order = tree_floor_lookup(rnode, id);
  if(!order) {
    printf("ID [%f] IS NOT FOUND IN TREE\n", id);
    return -1;
  }

  if(order->id!=id) {
    printf("NODE [%f] NOT FOUND!!!\n", id);
    printf("DO YOU MEAN ID=[%f] ??\n", order->id);
    return -1;
  }

  pnode = order->root;
  /*Update nodes before delete*/
  if((!order->right)&&(!order->left))
    if(pnode->id < order->id)
      pnode->right = order->right;
    else
      pnode->left = order->right;

  if(order->right) {
    order->right->root = pnode;
    if(pnode->id < order->id)
      pnode->right = order->right;
    else
      pnode->left = order->right;
  }

  if(order->left) {
    for(node=order->right;node;node=node->left)
      pnode = node;
    order->left->root = pnode;
    if(pnode->id < order->id)
      pnode->right = order->left;
    else
      pnode->left = order->left;
  }

  if(order->prev)
    order->prev->next = order->next;
  if(order->next)
    order->next->prev = order->prev;

  printf("DELETE ID [%f]\n", order->id);
  free(order);
  return 0;
}

/*
 * @brief : Get min number in tree, left side
 * @root  : root of the tree
 * @return: min node 
 */
tree_t *tree_get_min(tree_t *rnode) {
  tree_t *rpnode = rnode->right;
  tree_t *node = rnode;
  if(rpnode)
    for(node=rpnode;node->left;node=node->left);
  return node;
}

/*
 * @brief : Get max number in tree, right side
 * @root  : root of the tree
 * @return: max node 
 */
tree_t *tree_get_max(tree_t *rnode) {
  tree_t *rpnode = rnode->right;
  tree_t *node = rnode;
  if(rpnode) 
    for(node=rpnode;node->right;node=node->right);
  return node;
}

/*
 * @brief : Destroy entire tree
 * @root  : root of the tree
 * @return: None
 */
void tree_destroy(tree_t **rnode) {
  tree_t *node = *rnode;
  tree_t *rpnode = node->right;
  tree_t *order = node;
  tree_t *next = node;
  if(rpnode) {
    for(node=rpnode;node->left;node=node->left);
    for(order=node; order; order=next) {
      next=order->next;
      printf("Deleting node [%f]\n", order->id);
      free(order);
    }
  }
  free(*rnode);
  *rnode = NULL;
}

/*
 * @brief : inoder sort tree
 * @root  : root of the tree
 * @return: None
 */
int tree_inoder_sortup(tree_t *rnode,double (*sortup)(double)) {
  if(rnode == NULL)
    return 0;
  tree_inoder_sortup(rnode->left, sortup);
  sortup(rnode->id);
  tree_inoder_sortup(rnode->right,sortup);
}

double sort_result(double id) {
  printf("[%.2f] ", id);
  return id;
}


dulieu_t sv[19] = {
  {
    .diem = 5.5,
    .hoten = "TRINH VAN KHAI",
  },
  {
    .diem = 6,
    .hoten = "LE TRONG TAN",
  },
  {
    .diem = 4,
    .hoten = "Nguyen Van A",
  },
  {
    .diem = 7,
    .hoten = "Nguyen Van B",
  },
  {
    .diem = 3,
    .hoten = "Nguyen Van C",
  },
  {
    .diem = 8.7,
    .hoten = "Nguyen Van D",
  },
  {
    .diem = 6.5,
    .hoten = "Nguyen Van E",
  },
  {
    .diem = 6.4,
    .hoten = "Nguyen Van F",
  },
  {
    .diem = 6.6,
    .hoten = "Nguyen Van G",
  },
  {
    .diem = 6.9,
    .hoten = "Nguyen Van H",
  },
  {
    .diem = 6.8,
    .hoten = "Nguyen Van I",
  },
  {
    .diem = 1.2,
    .hoten = "Nguyen Van J",
  },
  {
    .diem = -4,
    .hoten = "Nguyen Van K",
  },
  {
    .diem = -32,
    .hoten = "Nguyen Van L",
  },
  {
    .diem = -6.5,
    .hoten = "Nguyen Van M",
  },
  {
    .diem = 3.4,
    .hoten = "Nguyen Van N",
  },
  {
    .diem = 0.56,
    .hoten = "Nguyen Van O",
  },
  {
    .diem = 12.31,
    .hoten = "Nguyen Van P",
  },
  {
    .diem = 11,
    .hoten = "Nguyen Van Q",
  },
};

tree_t *tree_inoder_upward(tree_t *rnode) {
  tree_t *rpnode = rnode->right;
  tree_t *node = rpnode;
  tree_t *order = node;
  dulieu_t *sv;
  if(rpnode)
    for(node=rpnode;node->left;node=node->left);
  printf("\n>>>FORWARD<<<\n");
  for(order=node; order; order=order->next) {
    sv = order->sv;
    if(sv) {
      printf("HO TEN: %s, DIEM [%.2f] \n", sv->hoten, sv->diem);
    }
  }
  printf("\n===END===\n");
}

tree_t *tree_inoder_downward(tree_t *rnode) {
  tree_t *rpnode = rnode->right;
  tree_t *node = rpnode;
  tree_t *order = node;
  dulieu_t *sv;
  if(rpnode)
    for(node=rpnode;node->right;node=node->right);
  printf("\n>>>BACKWARD<<<\n");
  for(order=node; order; order=order->prev) {
    sv = order->sv;
    if(sv) {
      printf("HO TEN: %s, DIEM [%.2f] \n", sv->hoten, sv->diem);
    }
  }
  printf("\n===END===\n");
}

int tree_search_range(tree_t *rnode, double lower, double upper) {
  tree_t *pnode = tree_ceiling_lookup(rnode, lower);
  tree_t *node=pnode;
  int i=0;
  if(!pnode) {
    printf("Not found, specified out of range\n");
    return i;
  }
  if(node->id>upper) {
    printf("Out of range, max is [%f]\n",node->id);
    return i;
  }
  printf("\n\n[%f, %f] = {", lower, upper);
  for(node=pnode; node; node=node->next) {
    if(node->id > upper) {
      break;
    } else {
      i++;
    }
    printf("%f ",node->id);
  }
  printf("}\n\n");

  return i;
}

int tree_search_from(tree_t *rnode, double lower, int exnumb) {
  tree_t *pnode = tree_ceiling_lookup(rnode, lower);
  tree_t *node=pnode;
  int i=0;
  if(!pnode) {
    printf("Not found, specified out of range\n");
    return i;
  }

  printf("\n\n[%d] = {", exnumb);
  for(node=pnode; (node)&&(i++<exnumb); node=node->next) {
    printf("%f ",node->id);
  }
  printf("}\n\n");
  return i;
}

int tree_search_upto(tree_t *rnode, double upper, int exnumb) {
  tree_t *pnode = tree_floor_lookup(rnode, upper);
  tree_t *node=pnode;
  int i=0;
  if(!pnode) {
    printf("Not found, specified out of range\n");
    return i;
  }

  printf("\n\n[%d] = {", exnumb);
  for(node=pnode; (node)&&(i++<exnumb); node=node->prev) {
    printf("%f ",node->id);
  }
  printf("}\n\n");
  return i;
}

int main (int argc, char *argv[]) {
  tree_t *node;
  int i;
  double lower, upper;
  lower = 2;
  upper = 8;
  tree_root_init(&treehead);
  tree_put_id(treehead, sv[0].diem, &sv[0]);
  tree_put_id(treehead, sv[1].diem, &sv[1]);
  tree_put_id(treehead, sv[2].diem, &sv[2]);
  tree_put_id(treehead, sv[3].diem, &sv[3]);
  tree_put_id(treehead, sv[4].diem, &sv[4]);
  tree_put_id(treehead, sv[5].diem, &sv[5]);
  tree_put_id(treehead, sv[6].diem, &sv[6]);
  tree_put_id(treehead, sv[7].diem, &sv[7]);
  tree_put_id(treehead, sv[8].diem, &sv[8]);
  tree_put_id(treehead, sv[9].diem, &sv[9]);
  tree_put_id(treehead, sv[10].diem, &sv[10]);
  tree_put_id(treehead, sv[11].diem, &sv[11]);
tree_put_id(treehead, sv[12].diem, &sv[12]);
tree_put_id(treehead, sv[13].diem, &sv[13]);
tree_put_id(treehead, sv[14].diem, &sv[14]);
tree_put_id(treehead, sv[15].diem, &sv[15]);
tree_put_id(treehead, sv[16].diem, &sv[16]);
tree_put_id(treehead, sv[17].diem, &sv[17]);
tree_put_id(treehead, sv[18].diem, &sv[18]);

  tree_inoder_upward(treehead);
  tree_inoder_downward(treehead);
  i = tree_search_range(treehead,lower,upper); //Tim so trong dai
  if(i)
    printf("FOUND [%d] NUMBERS IN RANGE[%f,%f]\n",i,lower,upper);
  else
    printf("THERE IS NO NUMBER IN RANGE[%f,%f]\n",lower,upper);

  tree_search_from(treehead, lower, 5); //Tim 5 so bat dau tu lower
  tree_search_upto(treehead, upper, 5);
  tree_del_id(treehead, sv[10].diem);
  tree_del_id(treehead, 3);
  tree_del_id(treehead, 20);
  tree_del_id(treehead, sv[3].diem);
  tree_del_id(treehead, sv[4].diem);
  tree_del_id(treehead, sv[5].diem);
  tree_del_id(treehead, sv[6].diem);
  i = tree_search_range(treehead,lower,upper);
  if(i)
    printf("FOUND [%d] NUMBERS IN RANGE[%f,%f]\n",i,lower,upper);
  else
    printf("THERE IS NO NUMBER IN RANGE[%f,%f]\n",lower,upper);
  tree_inoder_upward(treehead);
  tree_inoder_downward(treehead);

  tree_destroy(&treehead);
  return 0;
}



