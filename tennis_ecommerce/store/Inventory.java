/*
 * Authored-by: Yousef Yassin  
 * Last Edited: March 21, 2021
 */
package store;

import java.util.HashMap;
import java.util.LinkedHashMap;

/**
 * Class that represents a store's inventory. Provides functionality
 * to manage product inventory stock and gain access to product info.
 * @author Yousef Yassin < >
 * @version 2.0
 */
public class Inventory extends ProductStockContainer {
    private final HashMap<Integer, Product> products;   //map id to product

    /**
     * Creates new inventory instance with supplied default products (empty if null).
     * @param initProducts HashMap<Product, Integer>, product and stock pairs to initialize inventory.
     */
    public Inventory(HashMap<Product, Integer> initProducts) {
        super();
        this.products = new HashMap<>();
        if (initProducts != null) initInventory(initProducts);
    }

    /**
     * Initializes this inventory to a supplied set of products and corresponding quantities.
     * @param initProducts HashMap<Product, Integer>, product and stock pairs to initialize inventory.
     */
    private void initInventory(HashMap<Product, Integer> initProducts){
        int quantity;
        int id;
        Product product;

        for (HashMap.Entry<Product, Integer> item : initProducts.entrySet()){
            product = item.getKey();
            quantity = item.getValue();
            if (quantity < 0) continue;

            id = product.getId();

            this.products.put(id, product);
            this.getStock().put(id, quantity);
        }
    }

    /**
     * Add specified stock quantity to supplied product.
     * @param product Product, product to update inventory stock.
     * @param quantity Integer, amount to add to product's stock.
     */
    public void addProductQuantity(Product product, int quantity) {
        int id = product.getId();

        if (this.getStock().containsKey(id)) {                    //Product exists
            this.getStock().put(id, this.getStock().get(id) + quantity);
        } else {                                        //Product doesn't exist yet
            this.products.put(id, product);
            this.getStock().put(id, quantity);
        }
    }

    /**
     * Remove specified stock quantity from supplied product.
     * @param product Product, product from which to remove stock.
     * @param quantity Integer, amount to remove from product's stock.
     * @return boolean, the status of the remove operation, true
     *         if success, false otherwise.
     */
    public boolean removeProductQuantity(Product product, int quantity) {
        int currentStock;
        int id = product.getId();

        if (this.getStock().containsKey(id)) {                    //Product exists
            currentStock = this.getStock().get(id);

            if (currentStock >= quantity) {             //Sufficient stock to remove
                this.getStock().put(id, currentStock - quantity);
                return true;
            }
        }

        return false;
    }

    /**
     * Get product with specified id.
     * @param id Integer, id of product for which to obtain info.
     * @return Product, container containing the product's info (name, id and price or null
     *         if product does not exist).
     */
    public Product getProductInfo(int id) {
        return this.products.get(id);    //returns null if not found
    }

    /**
     * Get all products and associated stock in inventory.
     * @return HashMap<Product, Integer>, product stock pairs available in inventory.
     */
    public LinkedHashMap<Product, Integer> getAllStock(){
        LinkedHashMap<Product, Integer> allStock = new LinkedHashMap<>();

        for (HashMap.Entry<Integer, Product> item : this.products.entrySet()){
            Product product = item.getValue();
            allStock.put(product, this.getStock().get(product.getId()));
        }

        return allStock;
    }
}
