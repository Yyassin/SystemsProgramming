/*
 * Authored-by: Yousef Yassin  
 * Last Edited: April 4, 2021
 */

package store;

import java.util.HashMap;
import java.util.LinkedHashMap;

/**
 * Class representing a store management system. The Store Manager manages the carts of
 * all users connected to the store, as well as the store inventory.
 * @author Yousef Yassin  
 * @version 2.0
 */
public class StoreManager {
    private static int cartCounter = 0;                     //Store cartID of next user connection
    private final HashMap<Integer, ShoppingCart> carts;     //Manage state of all user connections: map id to cart
    private final Inventory inventory;                      //Store inventory instance

    /**
     * Creates a new StoreManager instance with empty default inventory.
     */
    public StoreManager() {
        this(null);
    }

    /**
     * Creates a new StoreManager instance with supplied default inventory.
     * @param products HashMap<Product, Integer>, product and stock pairs to initialize inventory.
     */
    public StoreManager(HashMap<Product, Integer> products) {
        this.inventory = new Inventory(products);
        this.carts = new HashMap<>();
    }

    /**
     * Generates unique cartID for each user connection.
     * @return Integer, the assigned cartID.
     */
    public int generateCartID() {
        int id = cartCounter++;
        this.carts.put(id, new ShoppingCart());
        return id;
    }

    /**
     * Adds specified quantity of a product to a user's cart.
     * @param cartID Integer, id of user's cart adding product.
     * @param productID Integer, id of product adding to the cart.
     * @param quantity Integer, amount of product to add to the cart.
     * @return boolean, the operation status; true if success or false otherwise.
     */
    public boolean addToCart(int cartID, int productID, int quantity){
        Product product = this.inventory.getProductInfo(productID);
        if (product == null || quantity <= 0) return false;

        boolean success = this.inventory.removeProductQuantity(product, quantity);

        if (success) this.carts.get(cartID).addProductQuantity(product, quantity);
        return success;
    }

    /**
     * Removes specified quantity of a product from a user's cart.
     * @param cartID Integer, id of user's cart removing product.
     * @param productID Integer, id of product being removed.
     * @param quantity Integer, amount of product to remove from the cart.
     * @return boolean, the operation status; true if success or false otherwise.
     */
    public boolean removeFromCart(int cartID, int productID, int quantity){
        Product product = this.inventory.getProductInfo(productID);

        if (product == null || quantity <= 0) return false;

        boolean success = this.carts.get(cartID).removeProductQuantity(product, quantity);

        if (success) this.inventory.addProductQuantity(product, quantity);
        return success;
    }

    /**
     * Get the amount of stock for a supplied product.
     * @param product Product, product for which to obtain stock.
     * @return Integer, amount of stock for given product.
     */
    public int getStock(Product product) { return this.inventory.getProductQuantity(product); }

    /**
     * Get the amount of stock for a supplied product Id.
     * @param productID int, product id for which to obtain stock.
     * @return Integer, amount of stock for given product.
     */
    public int getStock(int productID) { return this.inventory.getProductQuantity(this.inventory.getProductInfo(productID)); }

    /**
     * Get the total price for a supplied cart.
     * @param cartID int, cart id for which to obtain total.
     * @return Double, the total price for the items in the supplied cart.
     */
    public double getCartTotal(int cartID) {
        int[][] items = this.carts.get(cartID).getCartItems();
        if (items.length == 0) return 0;

        double total = 0;
        for (int[] item : items) total += item[1] * this.inventory.getProductInfo(item[0]).getPrice();
        return total;
    }

    /**
     * Process a checkout transaction for a user, prints checkout summary.
     * @param cartID Integer, id of user's cart to checkout.
     * @return double, total price of the transaction.
     */
    public double checkout(int cartID) {
        int[][] transaction = this.carts.get(cartID).getCartItems();
        if (transaction.length == 0) return -1;

        int id;
        int quantity;
        double price;
        double total = 0;
        Product product;

        System.out.printf("%8s | %15s | %5s \n", "Amount", "Product Name", "Unit Price");

        for (int[] item : transaction) {
            id = item[0];
            quantity = item[1];
            product = this.inventory.getProductInfo(id);
            price = product.getPrice();

            total += quantity * price;

            System.out.format("%8s | %15s | $ %5s \n",
                    quantity, product.getName(), price);
        }

        System.out.println(String.format("Total: $ %.2f", total));

        this.carts.replace(cartID, new ShoppingCart());

        return total;
    }

    /**
     * Process clean up for user's cart when they quit. Returns all items in cart to inventory and
     * disables user connection.
     * @param cartID Integer, id of quitting user's cart.
     */
    public void quit(int cartID){
        int[][] items = this.carts.get(cartID).getCartItems();

        for (int[] item : items){
            this.inventory.addProductQuantity(this.inventory.getProductInfo(item[0]), item[1]);
        }

        this.carts.remove(cartID);  //end the user's connection
    }

    /**
     * Get info -- products and stock -- of all products currently in the inventory.
     * @return HashMap<Product, Integer>, product stock pairs available in inventory.
     */
    public LinkedHashMap<Product, Integer> getInventoryInfo(){ return this.inventory.getAllStock(); }

    /**
     * Get items and quantity of all items in a user's cart.
     * @param cartID Integer, id of user's cart for which to obtain info.
     * @return HashMap<Product, Integer>, product stock pairs available in supplied user's cart.
     */
    public HashMap<Product, Integer> getCartInfo(int cartID){
        HashMap<Product, Integer> cartItemsMap = new HashMap<>();

        int[][] cartItems = this.carts.get(cartID).getCartItems();

        //cart only has ids, get products from inventory
        for (int[] item : cartItems){
            cartItemsMap.put(this.inventory.getProductInfo(item[0]), item[1]);
        }

        return cartItemsMap;
    }
}
