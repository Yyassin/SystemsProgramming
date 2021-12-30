/*
 * Authored-by: Yousef Yassin  
 * Last Edited: April 14, 2021
 */
package store;

import java.util.HashMap;

/**
 * Class that implements a container tracking products and their corresponding stock.
 * @author Yousef Yassin  
 * @version 1.0
 * @version 1.0
 */
public abstract class ProductStockContainer {
    private final HashMap<Integer, Integer> stock;      //Map product IDs to stock

    /**
     * Creates a new empty product stock container.
     */
    public ProductStockContainer() {
        this.stock = new HashMap<>();
    }

    /**
     * Get this container's stock mapping for all products.
     * @return HashMap<Integer, Integer>, the stock mapping.
     */
    protected HashMap<Integer, Integer> getStock() { return this.stock; }

    /**
     * Get the stock quantity for a supplied product.
     * @param product Product, the supplied product.
     * @return int, the stock quantity.
     */
    public int getProductQuantity(Product product){ return this.stock.getOrDefault(product.getId(), -1); }

    /**
     * Get the total number of unique products in this container.
     * @return int, the total number of unique products in this container.
     */
    public int getNumOfProducts() {
        return this.stock.size();
    }

    /**
     * Add specified quantity of given product to container.
     * @param product Product, product to add.
     * @param quantity Integer, amount of product to add.
     */
    public abstract void addProductQuantity(Product product, int quantity);

    /**
     * Remove a specified quantity of given product from contaienr.
     * @param product Product, product to remove quantity.
     * @param quantity Integer, amount of product to remove.
     * @return boolean, operation status; true if success, fail otherwise.
     */
    public abstract boolean removeProductQuantity(Product product, int quantity);
}
