/*
 * Authored-by: Yousef Yassin  
 * Last Edited: March 21, 2021
 */
package store;

import java.util.HashMap;

/**
 * Class representing a user's cart. A user holds an amount of items they wish to buy
 * in the cart: they can add and remove these items.
 * @author Yousef Yassin  
 * @version 1.0
 */
public class ShoppingCart extends ProductStockContainer{

    /**
     * Create new shopping cart object (empty).
     */
    public ShoppingCart(){
        super();
    }

    /**
     * Add specified quantity of given product to cart.
     * @param product Product, product to add to cart.
     * @param quantity Integer, amount of product to add to cart.
     */
    public void addProductQuantity(Product product, int quantity){
        int productID = product.getId();
        this.getStock().put(productID, this.getStock().getOrDefault(productID, 0) + quantity);
    }

    /**
     * Remove a specified quantity of given product from cart.
     * @param product Product, product to remove quantity from cart.
     * @param quantity Integer, amount of product to remove from cart.
     * @return boolean, operation status; true if success, fail otherwise.
     */
    public boolean removeProductQuantity(Product product, int quantity){
        int productID = product.getId();

        if (!this.getStock().containsKey(productID)) return false;
        int updatedQuantity = this.getStock().get(productID) - quantity;
        if (updatedQuantity < 0) return false;

        if (updatedQuantity == 0){
            this.getStock().remove(productID);
        } else {
            this.getStock().replace(productID, updatedQuantity);
        }

        return true;
    }

    /**
     * Get ids and quantity of all products in this cart.
     * @return int[][], containing product id(0) and quantity pairs(1) for all items in cart.
     */
    public int[][] getCartItems(){
        int[][] items = new int[this.getStock().size()][2];
        int i = 0;

        for (int id : this.getStock().keySet()){
            items[i++] = new int[]{id, this.getStock().get(id)};
        }

        return items;
    }
}
